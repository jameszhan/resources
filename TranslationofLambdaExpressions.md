Translation of Lambda Expressions
=================================

#### April 2012

### About this document


This document outlines the strategy for translating lambda expressions and method references from Java source code into bytecode. Lambda expressions for Java are being specified by [JSR 335][jsr335] and implemented in the OpenJDK [Lambda Project][lambda]. An overview of the language feature can be found in [State of the Lambda][lambdastate].

This document deals with how we arrive at the bytecode that the compiler must generate when it encounters lambda expressions, and how the language runtime participates in evaluating lambda expressions. The majority of this document deals with the mechanics of functional interface conversion.

Functional interfaces are a central aspect of lambda expressions in Java. A functional interface is an interface that has one non-**Object** method, such as **Runnable**, **Comparator**, etc. (Java libraries have used such interfaces to represent callbacks for years.)

Lambda expressions can only appear in places where they will be assigned to a variable whose type is a functional interface. For example:


`Runnable r = () -> { System.out.println("hello"); };`

or

`Collections.sort(strings, (String a, String b) -> -(a.compareTo(b)));`

The code that the compiler generates to capture these lambda expressions is dependent both on the lambda expression itself, and the functional interface type to which it is being assigned.

### Dependencies and notation

The translation scheme relies on several features from [JSR 292][jsr292], including invokedynamic, method handles and the enhanced LDC bytecode forms for method handles and method types. Because these are not representable in Java source code, our examples will use a pseudo-syntax for these features:

* For method handle constants: `MH([refKind] class-name.method-name)`
* For method type constants: `MT(method-signature)`
* For invokedynamic: `INDY((bootstrap, static args...)(dynamic args...))`


Readers should have a working knowledge of the [JSR 292][jsr292] features.

The translation scheme also assumes a new feature that is in the process of being specified by the JSR-292 Expert Group for Java SE 8: an API for reflection over constant method handles.

### Translation strategy

There are a number of ways we might represent a lambda expression in bytecode, such as inner classes, method handles, dynamic proxies, and others. Each of these approaches has pros and cons. In selecting a strategy, there are two competing goals: maximizing flexibility for future optimization by not committing to a specific strategy, vs providing stability in the classfile representation. We can achieve both of these goals by using the invokedynamic feature from [JSR 292][jsr292] to separate the binary representation of lambda creation in the bytecode from the mechanics of evaluating the lambda expression at runtime. Instead of generating bytecode to create the object that implements the lambda expression (such as calling a constructor for an inner class), we describe a recipe for constructing the lambda, and delegate the actual construction to the language runtime. That recipe is encoded in the static and dynamic argument lists of an **invokedynamic** instruction.

The use of **invokedynamic** lets us defer the selection of a translation strategy until run time. The runtime implementation is free to select a strategy dynamically to evaluate the lambda expression. The runtime implementation choice is hidden behind a standardized (i.e., part of the platform specification) API for lambda construction, so that the static compiler can emit calls to this API, and JRE implementations can choose their preferred implementation strategy. The **invokedynamic** mechanics allow this to be done without the performance costs that this late binding approach might otherwise impose.

When the compiler encounters a lambda expression, it first lowers (desugars) the lambda body into a method whose argument list and return type match that of the lambda expression, possibly with some additional arguments (for values captured from the lexical scope, if any.) At the point at which the lambda expression would be captured, it generates an **invokedynamic** call site, which, when invoked, returns an instance of the functional interface to which the lambda is being converted. This call site is called the lambda factory for a given lambda. The dynamic arguments to the lambda factory are the values captured from the lexical scope. The bootstrap method of the lambda factory is a standardized method in the Java language runtime library, called the lambda metafactory. The static bootstrap arguments capture information known about the lambda at compile time (the functional interface to which it will be converted, a method handle for the desugared lambda body, information about whether the SAM type is serializable, etc.)

Method references are treated the same way as lambda expressions, except that most method references do not need to be desugared into a new method; we can simply load a constant method handle for the referenced method and pass that to the metafactory.

### Lambda body desugaring

The first step of translating lambdas into bytecode is desugaring the lambda body into a method.

There are several choices that must be made surrounding desugaring:

* Do we desugar to a static method or instance method?
* In what class should the desugared method go?
* What should be the accessibility of the desugared method?
* What should be the name of the desugared method?
* If adaptations are required to bridge differences between the lambda body signature and the functional interface method signature (such as boxing, unboxing, primitive widening or narrowing conversions, varargs conversions, etc), should the desugared method follow the signature of the lambda body, the functional interface method, or something in between? Who is responsible for the needed adaptations?
* If the lambda captures arguments from the enclosing scope, how should those be represented in the desugared method signature? (They could be individual arguments added to the beginning or end of the argument list, or the compiler could collect them into a single "frame" argument.)


Related to the issue of desugaring lambda bodies is that of whether method references require the generation of an adapter or "bridge" method.

The compiler will infer a method signature for the lambda expression, including argument types, return type, and thrown exceptions; we will call this the natural signature. Lambda expressions also have a target type, which will be a functional interface; we will call the lambda descriptor the method signature for the descriptor of the erasure of the target type. The value returned from the lambda factory, which implements the functional interface and captures the behavior of the lambda, is called the lambda object.

All things being equal, private methods are preferable to nonprivate, static methods preferable to instance methods, it is best if lambda bodies are desugared into in the innermost class in which the lambda expression appears, signatures should match the body signature of the lambda, extra arguments should be prepended on the front of the argument list for captured values, and would not desugar method references at all. However, there are exception cases where we may have to deviate from this baseline strategy.

### Desugaring example -- "stateless" lambdas

The simplest form of lambda expression to translate is one that captures no state from its enclosing scope (a stateless lambda):

```
class A {
    public void foo() {
        List<String> list = ...
        list.forEach( s -> { System.out.println(s); } );
    }
}
```

The natural signature of the lambda is `(String)V`; the forEach method takes a `Block<String>` whose lambda descriptor is `(Object)V`. The compiler desugars the lambda body into a static method whose signature is the natural signature, and generates a name for the desugared body.

```
class A {
    public void foo() {
        List<String> list = ...
        list.forEach( [lambda for lambda$1 as Block] );
    }

    static void lambda$1(String s) {
        System.out.println(s);
    }
}
```

### Desugaring example -- lambdas capturing immutable values

The other form of lambda expression involves capture of enclosing final (or effectively final) local variables, and/or fields from enclosing instances (which we can treat as capture of the final enclosing `this` reference).

```
class B {
    public void foo() {
        List<Person> list = ...
        final int bottom = ..., top = ...;
        list.removeIf( p -> (p.size >= bottom && p.size <= top) );
    }
}
```

Here, our lambda captures the final local variables bottom and top from the enclosing scope.

The signature of the desugared method will be the natural signature `(Person)Z` with some extra arguments prepended at the front of the argument list. The compiler has some latitude as to how these extra arguments are represented; they could be prepended individually, boxed into a frame class, boxed into an array, etc. The simplest approach is to prepend them individually:

```
class B {
    public void foo() {
        List<Person> list = ...
        final int bottom = ..., top = ...;
        list.removeIf( [ lambda for lambda$1 as Predicate capturing (bottom, top) ]);
    }

    static boolean lambda$1(int bottom, int top, Person p) {
        return (p.size >= bottom && p.size <= top;
    }
}
```

Alternately, the captured values (bottom and top) could be boxed into a frame or an array; the key is agreement between the types of the extra arguments as they appear in the signature of the desugared lambda method, and the types as they appear as (dynamic) arguments to the lambda factory. Since the compiler is in control of both of these, and they are generated at the same time, the compiler has some flexibility in how captured arguments are packaged.

### The Lambda Metafactory

Lambda capture will be implemented by an **invokedynamic** call site, whose static parameters describe the characteristics of the lambda body and lambda descriptor, and whose dynamic parameters (if any) are the captured values. When invoked, this call site returns a lambda object for the corresponding lambda body and descriptor, bound to the captured values. The bootstrap method for this callsite is a specified platform method called the lambda metafactory. (We can have a single metafactory for all lambda forms, or have specialized versions for common situations.) The VM will call the metafactory only once per capture site; thereafter it will link the call site and get out of the way. Call sites are linked lazily, so factory sites that are never invoked are never linked. The static argument list for the basic metafactory looks like:

```
metaFactory(MethodHandles.Lookup caller, // provided by VM
            String invokedName,          // provided by VM
            MethodType invokedType,      // provided by VM
            MethodHandle descriptor,     // lambda descriptor
            MethodHandle impl)           // lambda body
```
            
The first three arguments (`caller`, `invokedName`, `invokedType`) are automatically stacked by the VM at callsite linkage.

The `descriptor` argument identifies the functional interface method to which the lambda is being converted. (Through a reflection API for method handles, the metafactory can obtain the name of the functional interface class and the name and method signature of its primary method.)

The `impl` argument identifies the lambda method, either a desugared lambda body or the method named in a method reference.

There may be some differences between the method signatures for the functional interface method and the implementation method. The implementation method may have extra arguments corresponding to captured arguments. The remaining arguments also may not match exactly; certain adaptations (subtyping, boxing) are permitted as described in Adaptations.

### Lambda capture

We are now ready to describe the translation of functional interface conversion for lambda expressions and method references. We can translate example A as:

```
class A {
    public void foo() {
        List<String> list = ...
        list.forEach(indy((MH(metaFactory), MH(invokeVirtual Block.apply), MH(invokeStatic A.lambda$1)( )));
    }

    private static void lambda$1(String s) {
        System.out.println(s);
    }
}
```

Because the lambda in `A` is stateless, the dynamic argument list of the lambda factory site is empty.

For example `B`, the dynamic argument list is not empty, because we must provide the values of bottom and top to the lambda factory:

```
class B {
    public void foo() {
        List<Person> list = ...
        final int bottom = ..., top = ...;
        list.removeIf(indy((MH(metaFactory), MH(invokeVirtual Predicate.apply),
                            MH(invokeStatic B.lambda$1))( bottom, top ))));
    }

    private static boolean lambda$1(int bottom, int top, Person p) {
        return (p.size >= bottom && p.size <= top;
    }
}
```

### Static vs instance methods

Lambdas like those in the above section can be translated to static methods, since they do not use the enclosing object instance in any way (do not refer to `this`, `super`, or members of the enclosing instance.) Collectively, we will refer to lambdas that use `this`, `super`, or capture members of the enclosing instance as instance-capturing lambdas.

Non-instance-capturing lambdas are translated to private, static methods. Instance-capturing lambdas are translated to private instance methods. This simplifies the desugaring of instance-capturing lambdas, as names in the lambda body will mean the same as names in the desugared method, and meshes well with available implementation techniques (bound method handles.) When capturing an instance-capturing lambda, the receiver (`this`) is specified as the first dynamic argument.

As an example, consider a lambda that captures a field `minSize`:

`list.filter(e -> e.getSize() < minSize )`

We desugar this as an instance method, and pass the receiver as the first captured argument:

```
list.forEach(INDY((MH(metaFactory), MH(invokeVirtual Predicate.apply),
                   MH(invokeVirtual B.lambda$1))( this ))));

private boolean lambda$1(Element e) {
    return e.getSize() < minSize;
}
```

Because lambda bodies are translated to private methods, when passing the behavior method handle to the metafactory, the capture site should load a constant method handle whose reference kind is `REF_invokeSpecial` for instance methods and `REF_invokeStatic` for static methods.

We can desugar to private methods because the private method is accessible to the capturing class, and therefore can obtain a method handle to the private method which is then invokable by the metafactory. (If the metafactory is generating bytecode to implement the target functional interface, rather than invoking the method handle directly, it will load these classes through `Unsafe.defineClass` which is immune to accessibility checks.)

### Method reference capture

There are multiple forms of method references, which, like lambdas, can be divided into instance-capturing and non-instance-capturing. Non-instance-capturing method reference include static method references (`Integer::parseInt`, captured with reference kind invokeStatic), unbound instance method references (`String::length`, captured with reference kind invokeVirtual), and top-level constructor references (`Foo::new,` captured with reference kind invoke_newSpecial). When capturing a non-instance-capturing method reference, the captured argument list is always empty:

`list.filter(String::isEmpty)`

is translated as

`list.filter(indy(MH(metaFactory), MH(invokeVirtual Predicate.apply), MH(invokeVirtual String.isEmpty))()))`


Instance-capturing method reference forms include bound instance method references (`s::length`, captured with reference kind invokeVirtual), super method references (`super::foo`, captured with reference kind invokeSpecial), and inner class constructor references (`Inner::new`, captured with reference kind invokeNewSpecial). When capturing an instance-capturing method reference, the captured argument list always has a single argument, which is `this` in the case of super or inner class constructor method references, and the specified receiver for bound instance method references.

### Varargs

If a method reference to a varargs method is being converted to a functional interface that is not a varargs method, the compiler must generate a bridge method and capture a method handle for the bridge instead of the target method itself. The bridge must handle any needed adaptations of argument types as well as converting from varargs to non-varargs. For example:

```
interface IIS {
    void foo(Integer a1, Integer a2, String a3);
}

class Foo {
    static void m(Number a1, Object... rest) { ... }
}

class Bar {
    void bar() {
        SIS x = Foo::m;
    }
}
```

Here, the compiler needs to generate a bridge to perform adaptation of the first argument type from `Number` to `Integer`, and the gathering of the remaining arguments into an Object array:

```
class Bar {
    void bar() {
        SIS x = indy((MH(metafactory), MH(invokeVirtual IIS.foo),
                      MH(invokeStatic m$bridge))( ))
    }

    static private void m$bridge(Integer a1, Integer a2, String a3) {
        Foo.m(a1, a2, a3);
    }
}
```

### Adaptations

The desugared lambda method has an argument list and a return type: `(A1..An) -> Ra` (if the desugared method is an instance method, the receiver is considered to be the first argument). The functional interface method similarly has an argument list and return type: `(F1..Fm) -> Rf` (no receiver argument). And the dynamic argument list to the factory site has argument types `(D1..Dk)`. If the lambda is instance-capturing, the first dynamic argument must be the receiver.

The lengths of these must add up as follows: `k+m == n`. That is, the lambda body argument list should be as long as the dynamic argument list and the functional interface method argument list combined.

We split the lambda body argument list `A1..An` into `(D1..Dk H1..Hm)`, where the D arguments correspond to the "extra" (dynamic) arguments and the H arguments correspond to the functional interface arguments.

We require that Hi be adaptable to `Fi` for `i` in `1..m`. Similarly we require that `Ra` be adaptable to `Rf`. Type T is adaptable to type U if:

* T == U
* T is a primitive type and U is a reference type, and T can be converted to U via a boxing conversion
* T is a reference type and U is a primitive type, and T can be converted to U via an unboxing conversion
* T and U are primitives, and T can be converted to U via a primitive widening conversion
* T and U are reference types, and T can be cast to U

Adaptations are validated by the metafactory at linkage time and performed by the metafactory at capture time.

### Metafactory variants

It is practical to have a single metafactory for all lambda forms. However, it seems preferable to divide the metafactory into more than one version:

* A "fast path" version, which supports nonserializable lambdas and nonserializable static or unbound instance method references;
* A "serializable" version, which supports serializable lambdas and method references of all kinds;
* If necessary, a "kitchen sink" version, which supports arbitrary combinations of conversion features.

The kitchen sink version would take an additional `flags` argument to select options, and possibly other option-specific arguments. The serializable version might take additional arguments related to serialization.

Since the metafactories are not invoked directly by the user, there is no confusion introduced by having multiple ways to do the same thing. By eliminating arguments where they are not needed, classfiles become smaller. And the fast path option lowers the bar for the VM to intrinsify the lambda conversion operation, enabling it to be treated as a "boxing" operation and faciliating unbox optimizations.

### Serialization

Our dynamic translation strategy mandates a dynamic serialization strategy. If we wish to be able to switch from, say, spinning an inner class per lambda today to using dynamic proxies tomorrow, then lambda objects serialized today have to turn into dynamic proxies when deserialized tomorrow. This can be accomplished by defining a neutral serialization form for lambda expressions, and using `readResolve` and `writeReplace` to convert between lambda objects and the serialized form. The serialized form would have to contain all the information needed to recreate the object through the metafactory. For example, the serialized form could look like this:

```
public interface SerializedLambda extends Serializable {
    // capture context
    String getCapturingClass();

    // SAM descriptor
    String getDescriptorClass();
    String getDescriptorName();
    String getDescriptorMethodType();

    // implementation
    int getImplReferenceKind();
    String getImplClass();
    String getImplName();
    String getImplMethodType();

    // dynamic args -- these will individually need to be Serializable too
    Object[] getCapturedArgs();
}
```

Here, the `SerializedLambda` interface provides all the information present at the original lambda capture site. When capturing a serializable lambda, the metafactory would have to return an object that implements the `writeReplace` method, returning a `SerializedLambda` implementation which has a `readResolve` method that is responsible for re-creating the lambda object.

### Accessibility

One challenge is that the deserialization code needs to construct a method handle for the implementation method. While the serialized form provides all the nominal information for doing so -- kind, class, name, and type -- and therefore could be constructed with one of the `findXxx` methods exposed on `MethodHandles.Lookup`, the lambda method or method being referenced might be inaccessible to the `SerializedLambda` implementation (either because the method is inaccessible or the enclosing class is inaccessible.)

This isn't a problem for the lambda factory site from which the lambda originated, since the method handle for the implementation is loaded using the accessibility privileges of that class. However, to avoid introducing security hazards, we would like to minimize any elevated privileges used in deserialization, and certainly would prefer to not modify the accessibility rules for the JVM.

One possibility is to ensure that the implementation method of a serializable lambda or method reference is a public method of a public class. This may already be true (method reference to `String::length`), or could easily be made true (for public classes, we can desugar lambdas to public methods). But this is also undesirable, in that it exposes internals as public methods, and would be inconsistent with our translation of nonserializable lambdas. It also requires some significant gymnastics in some cases, such as creating a public "sidecar" class if the lambda is in a nonpublic class. (To some degree, the "expose as public" aspect is unavoidable, as that is what serialization does -- provides an external public de-facto constructor for a class. But we would like to minimize this exposure.)

A better approach is to delegate deserialization back to the class doing the lambda capture. One key security challenge is to not let the deserialization mechanism allow an attacker to construct a lambda object that invokes an arbitrary private method by constructing a doctored bytestream. Serialization naturally exposes "constructors" for specific combinations of (functional interface, behavior method, captured arg types); by delegating back to the capturing class, it can verify that the bytestream represents one of the valid combinations before proceeding. Once it has validated the combination, it can construct the lambda through metafactory calls, and load method handles using its own accessibility priveleges.

To do this, the capturing class should have a helper method that can be called from the serialization layer, in the spirit of `readObject`, `writeObject`, `readResolve`, and `writeReplace`. We'll call this `$deserialize$(SerializedLambda)`. The only privileged operation needed by the serialization layer is to call this (probably private) method.

When compiling a class that captures serializable lambdas, the compiler knows which combination of (functional interface, behavior method, captured argument types) have been captured as serializable lambdas. The `$deserialize$` method should only support deserialization of these combinations.

Consider this class that captures two serializable lambdas:

```
class Foo {
    void moo() {
        SerializableComparator<String> byLength = (a,b) -> a.length() - b.length();
        SerializablePredicate<String> isEmpty = String::isEmpty;
        ...
    }
}
```

We can translate as follows:

```
class Foo {
    void moo() {
        SerializableComparator<String> byLength
            = indy(MH(serializableMetafactory), MH(invokeVirtual SerializableComparator.compare),
                   MH(invokeStatic lambda$1))());
        SerializablePredicate<String> isEmpty
            = indy(MH(serializableMetafactory), MH(invokeVirtual SerializablePredicate.apply),
                   MH(invokeVirtual String.isEmpty)());
        ...
    }

    private static int lambda$1(String a, String b) { return a.length() - b.length(); }

    private static $deserialize$(SerializableLambda lambda) {
        switch(lambda.getImplName()) {
        case "lambda$1":
            if (lambda.getSamClass().equals("com/foo/SerializableComparator")
                 && lambda.getSamMethodName().equals("compare")
                 && lambda.getSamMethodDesc().equals("...")
                 && lambda.getImpleReferenceKind() == REF_invokeStatic
                 && lambda.getImplClass().equals("com/foo/Foo")
                 && lambda.getImplDesc().equals(...)
                 && lambda.getInvocationDesc().equals(...))
                     return indy(MH(serializableMetafactory),
                                 MH(invokeVirtual SerializableComparator.compare),
                                 MH(invokeStatic lambda$1))(lambda.getCapturedArgs()));
            break;

        case "isEmpty":
            if (lambda.getSamClass().equals("com/foo/SerializablePredicate"))
                 && lambda.getSamMethodName().equals("apply")
                 && lambda.getSamMethodDesc().equals("...")
                 && lambda.getImpleReferenceKind() == REF_invokeVirtual
                 && lambda.getImplClass().equals("java/lang/String")
                 && lambda.getImplDesc().equals(...)
                 && lambda.getInvocationDesc().equals(...))
                     return indy(MH(serializableMetafactory),
                                 MH(invokeVirtual SerializablePredicate.apply),
                                 MH(invokeVirtual String.isEmpty)(lambda.getCapturedArgs));

            break;
        }
        throw new ...;
    }
}
```

The $deserialize$ method knows which lambdas have been captured by this class, so can check the provided serialized form against the list, and then reconstruct the lambda with an identical call site, which can share the same bootstrap index as the capture site. (Alternately, it can share the same actual capture site, and therefore the same linkage state, by factoring the capture into a private method; this may simplify some issues raised in Class Caching below.)

IF a malicious caller tricks us into deserializing a malicious bytestream, it will only work for methods that actually are the targets of lambda conversions in that compilation unit, which is what we'd exposed if we translated to serializable inner classes. Because it is in the same compilation unit as the desugared method, it introduces no additional name instability with respect to recompilation.

This allows a simple, one-size-fits-all desugaring strategy for lambda bodies -- we use the same strategy for both serializable and nonserializable lambdas. It preserves the ability to make all desugared lambda bodies private, eliminates the need for sidecar classes or accessibility bridge methods, and the only privileged operation is the calling of `$deserialize$`.

To reduce the exposure to class-loading attacks (where an attacker creates a serialized lambda description with the intention of forcing the class to be loaded for the side-effects of its static initializers) it is probably best if the SerializedLambda interface dealt entirely in nominal identifiers for class names rather than Class objects.

### Class caching

In a number of the possible translation strategies, we need to generate new classes. For example, if we are generating a class-per-lambda (spinning inner classes at runtime instead of compile time), we generate the class the first time a given lambda factory site is called. Thereafter, future calls to that lambda factory site will re-use the class generated on the first call.

For serializable lambdas, there are two points at which the class generation could be triggered: the capture site, and the corresponding factory site in the $deserialize$ code. It is desirable (though not required) that whichever path is triggered first, objects generated through either path have the same class, which requires a unique key for each lambda capture, and a cache that is shared between the two capture sites for a given serializable lambda.

```
class SerializationExperiment {
    interface Foo extends Serializable { int m(); }

    public static void main(String[] args) {
        Foo f1, f2;
        if (args[0].equals("r")) {
            // read file 'foo.ser' and deserialize into f1
        }

        f2 = () -> 3;

        if (args[0].equals("w")) {
            // serialize f2 and write into file 'foo.ser'
            // read file 'foo.ser' and deserialize into f1
        }

        assert f1.getClass() == f2.getClass();
    }
}
```

If we run this program twice:

```
java -ea SerializationExperiment w
java -ea SerializationExperiment r
```

It would be desirable if runs were to succeed, whether the class spinning happens on first deserialization or on first call to the metafactory.

### Performance impact

Serializability imposes some additional costs on lambdas, because the lambda objects need to carry enough state to effectively recreate the static and dynamic argument lists for the metafactory. This may mean extra fields in the implementation class, extra constructor initialization work, and constraints on the translation strategy (for example, we couldn't use method handle proxies, since the resulting object would not implement the required `writeReplace` method.) Therefore it is preferable to treat serializable lambdas separately rather than making all lambdas serializable, and imposing these costs on all lambdas.

### Miscellaneous

#### Bridge methods

A functional interface may actually have more than one non-`Object` method, because it may have bridge methods. For example, in the functional interface type B below:

```
interface A<T> { void m(T t); }

interface B extends A<String> { void m(String s); }
```

The primary method for `B` will be `m(String)`, but B will also have a method `m(Object)` which is a bridge method to `m(String)`. (Otherwise invocation would fail if you cast the B to an A and invoked m on the result.)

When we convert a lambda expression to an object implementing a functional interface such as B, we must make sure that all of the bridges are wired correctly as well as the primary method, with suitable argument or return type adapation (casting). It is possible, through malicious bytecode generation or separate compilation artifacts to also find "extra" methods that were not present in the functional interface at compilation time. Rather than perform the full JLS bridge computation algorithm and bridge only those, we can take the shortcut taken by `MethodHandleProxy`, which is to bridge all methods with the same name and arity as the primary method. (If it turns out any of these are not compatible with the primary method, a `ClassCastException` will occur at invocation time, which is only slightly less informative than the linkage error that would otherwise be thrown.) We could have the compiler include a list of the known-valid-at-compile-time bridge signatures in the metafactory, but this would increase the classfile size for little benefit.

#### toString

In general the toString method for lambda objects is inherited from Object. However, for method references to public nonsynthetic methods, we may wish to implement toString in terms of the the class and method name from the implementation method. For example, for String::size converted to an IntFn, we might have toString return `String::size()`, `java.lang.String::size()`, `String::size() as IntFn`, etc.

TODO: If we support the concept of named lambdas, we may wish the `toString` result to be derived from the name, in which case the name would have to be passed into the metafactory somehow.






[jsr335]: http://jcp.org/en/jsr/detail?id=335 "JSR 335"
[lambda]: http://openjdk.java.net/projects/lambda/ "Lambda Project"
[lambdastate]: http://cr.openjdk.java.net/~briangoetz/lambda/lambda-state-4.html "State of the Lambda"
[jsr292]: http://jcp.org/en/jsr/detail?id=292 "jsr292"
[sshkit]: https://github.com/capistrano/sshkit "SSHKit"
[net-ssh]: https://github.com/net-ssh/net-ssh "Net::SSH"

