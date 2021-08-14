
Syntax
======

Integer literal
---------------

Syntax:

```bnf
<int-tag>
  ::= <u?>i<8|16|32|64>
<int-literal>
  ::= <-?><digit>+<int-tag>
  ||= 0x<xdigit>+<int-tag>
```

Example

```txt
x = 0i32;
y = 1i64;
z = 0xFF32i64;
sys.console.println(x); // 0
sys.console.println(y); // 1
sys.console.println(z); // 65330
```

Float literal
-------------

Syntax:

```bnf
<fp-tag>
  ::= f32 | f64
<float-literal>
  ::= <-?><digit>+<fp-tag>
  ||= <-?><digit>*.<digit>+<fp-tag>
```

Example:

```txt
a = 0.1f32;
b = 3.14f64;
sys.console.println(a); // 3.140000
sys.console.println(b); // 0.100000
```

String literal
--------------

Syntax:

```bnf
<string-literal>
  ::= " <any-char>* "
```

Example:

```txt
ss = "foo";
sys.console.println(ss); // foo
```

Tuple literal
-------------

Syntax:

```bnf
<tuple-elements>
  ::= <expr><,<expr>>*
<tuple-literal>
  ::= (<tuple-elements>)
```

Example:

```txt
tup = (0i32, 1i32, 2i32, "aaaa", 3f32, 4i64);
a = tup[0];
sys.console.println(tup[0]) // 0
sys.console.println(tup[3]) // aaaa
```

Array literal
------------

Syntax:

```bnf
<array-elements>
  ::= <expr><,<expr>>*
<array-literal>
  ::= [<array-elements>]
```

Example:

```txt
li = [2, 3, 5];
```

Statement
---------

Syntax:

```bnf
<literal>
  ::= <int-literal>
  ||= <float-literal>
  ||= <string-literal>
  ||= <array-literal>
  ||= <tuple-literal>

<statement-list>
  ::= <statement>+
<ret-context>
  ::= {
    <statement-list>?
    ret <expr>;
  }
<if-expr>
  ::= if(<expr>) <expr> else <expr>
<unary-expr>
  ::= <unary-op> <expr>
<binary-expr>
  ::= <expr> <binary-op> <expr>
<expr>
  ::= <identifier>
  ||= <literal>
  ||= <unary-expr>
  ||= <binary-expr>
  ||= <func-call-expr>
  ||= <if-expr>
  ||= <ret-context>
<asn-left-hand>
  ::= <identifier>
  ||= _
<asn-left-hand-list>
  ::= <asn-left-hand><,<asn-left-hand>*>
<assign-stmt>
  ::= <asn-left-hand-list> = <expr>;
<statement>
  ::= <assign-stmt>
```

Function
--------

Syntax:

```bnf
<param>
  ::= <identifier> <=<expr>>?
<param-list>
  ::= <param><,<param>*>
<capture-term>
  ::= <identifier>
<capture-list>
  ::= [<capture-term><,<capture-term>*>]
<lambda-literal>
  ::= <capture-list>? ( <param-list>? ) <expr>
<param-bind>
  ::= <identifier> = <expr>
<param-bind-list>
  ::= <param-bind><,<param-bind>*>
<func-call-expr>
  ::= <identifier> (
    <param-bind-list>?
  )
```
