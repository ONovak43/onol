# Grammar

program         -> declaration* EOF;

declaration     -> interfaceDecl
                | structDecl
                | fnDecl
                | varDecl
                | statement ;

interfaceDecl   -> "interface" INDENTIFIER "{" interfaceDecl* "}" ;
interfaceBody   -> fnSig* ;
fnSig           -> INDENTIFIER "(" parameters? ")" ( ":" Type)? ;

structDecl      -> "struct" INDENTIFIER (":" INDENTIFIER ("," IDENTIFIER)*)* "{" structBody* "}" ;
structBody      -> (varDecl | fnDecl)* ;

fnDecl          -> "fn" function ;
function        -> IDENTIFIER "(" parameters? ")" (":" Type)? block ;
parameters      -> parameter ( "," parameter )* ;
parameter       -> ("mut"? Type IDENTIFIER) ;

varDecl         -> ("mut")? ( Type | let) IDENTIFIER ( "=" expression")?  ;

Type            -> "int" | "double" | "string" | "boolean" | IDENTIFIER ;

statement       -> exprStmt
                -> forStmt
                -> ifStmt
                -> returnStmt
                -> returnIfStmt
                -> block ;

exprStmt        -> expression ;

forStmt         -> "for" forControl statement ;
forControl      -> expression
                -> varDecl? ";" expression? ";" expression?
                | ("mut")? (Type | let) IDENTIFIER "in" expression ;

ifStmt          -> "if" expression statement ("else" statement)? ;

returnStmt      -> "return" expression? ;
returnIfStmt    -> "returnif" expression ;

block           -> "{" declaration* "}" ;

expression      -> assignment;
assignment      -> (call ".")? IDENTIFIER "=" assignment
                | logicOr ;

logicOr         -> logicAnd ("or" logicAnd)* ;
logicAnd        -> equality ("and" equality)* ;

equality        -> comparison (("!=" | "==") comparison)* ;
comparison      -> term ((">" | ">=" | "<" | "<=") term)* ;

term            -> factor (("-" | "+") factor)* ;
factor          -> unary (("/" | "*") unary)* ;
unary           -> ("!" | "-") unary | call ;

call            -> primary ("(" arguments? ")" | "." IDENTIFIER)* ;
primary         -> "true" | " false" | "nil" | "this" | NUMBER
                | STRING | IDENTIFIER | "(" expression ")"

arguments       -> expression ("," expression)* ;

NUMBER          -> DIGIT+ ("." DIGIT+)? ;
STRING          -> "\"" (<any char except "\"">)* "\"" ;
IDENTIFIER      -> ALPHA (ALPHA | DIGIT)* ;
ALPHA           -> "a" ... "z" | "A" ... "Z" | "_" ;
DIGIT           -> "0" ... "9" ;

