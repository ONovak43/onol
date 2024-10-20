# Grammar

<program> ::= <declaration>* "EOF"

<declaration> ::= <interfaceDecl>
                | <structDecl>
                | <fnDecl>
                | <varDecl>
                | <statement>

<interfaceDecl> ::= "interface" <IDENTIFIER> "{" <interfaceBody> "}"
<interfaceBody> ::= <fnSig>*
<fnSig> ::= <IDENTIFIER> "(" <parameters>? ")" ( ":" <Type> )?

<structDecl> ::= "struct" <IDENTIFIER> ( ":" <IDENTIFIER> ( "," <IDENTIFIER> )* )? "{" <structBody> "}"
<structBody> ::= ( <varDecl> | <fnDecl> )*

<fnDecl> ::= "fn" <function>
<function> ::= <IDENTIFIER> "(" <parameters>? ")" ( ":" <Type> )? <block>
<parameters> ::= <parameter> ( "," <parameter> )*
<parameter> ::= ( "mut"? <Type> <IDENTIFIER> )

<varDecl> ::= "mut"? ( <Type> | "let" ) <IDENTIFIER> ( "=" <expression> )?
<Type> ::= "int" | "double" | "string" | "boolean" | <IDENTIFIER>

<statement> ::= <exprStmt>
              | <forStmt>
              | <ifStmt>
              | <returnStmt>
              | <returnIfStmt>
              | <block>

<exprStmt> ::= <expression>

<forStmt> ::= "for" <forControl> "{" <statement> "}"

<forControl> ::= <expression>
               | ( "mut"? ( <Type> | "let" ) <IDENTIFIER> "in" <expression> )

<ifStmt> ::= "if" <expression> "{" <statement> "}" ( "else" "{" <statement> "}" )?

<returnStmt> ::= "return" <expression>?
<returnIfStmt> ::= "returnif" <expression>

<block> ::= "{" <declaration>* "}"

<expression> ::= <assignment>
<assignment> ::= ( <call> "." )? <IDENTIFIER> "=" <assignment>
               | <logicOr>
<logicOr> ::= <logicAnd> ( "or" <logicAnd> )*
<logicAnd> ::= <equality> ( "and" <equality> )*

<equality> ::= <comparison> ( ( "!=" | "==" ) <comparison> )*
<comparison> ::= <term> ( ( ">" | ">=" | "<" | "<=" ) <term> )*

<term> ::= <factor> ( ( "-" | "+" ) <factor> )*
<factor> ::= <unary> ( ( "/" | "*" ) <unary> )*

<unary> ::= ( "!" | "-" ) <unary> | <call>

<call> ::= <primary> ( "(" <arguments>? ")" | "." <IDENTIFIER> )*
<primary> ::= "true" | "false" | "nil" | "this" | <NUMBER>
            | <STRING> | <IDENTIFIER> | "(" <expression> ")"

<arguments> ::= <expression> ( "," <expression> )*

<NUMBER> ::= [0-9]+ ( "." [0-9]+ )?
<STRING> ::= "\"" ( <STRING_CHAR> )* "\""
<STRING_CHAR> ::= [a-z] | [A-Z] | [0-9] | " " | <SPECIAL_CHAR> | <ESCAPED_CHAR>

<SPECIAL_CHAR> ::= "!" | "#" | "$" | "%" | "&" | "'" | "(" | ")" | "*" | "+"
                | "," | "-" | "." | "/" | ":" | ";" | "<" | "=" | ">" | "?"
                | "@" | "[" | "]" | "^" | "_" | "`" | "{" | "|" | "}" | "~"

<ESCAPED_CHAR> ::= "\\\""
                 | "\\\\"
                 | "\n"
                 | "\t"

<IDENTIFIER> ::= ([a-z] | [A-Z] | "_") ([a-z] | [A-Z] | [0-9] | "_")*

# Keywords

- interface
- struct
- fn
- mut
- let
- for
- in
- if
- else
- return
- returnif
- true
- false
- nil
