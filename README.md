regex       ::= term { '|' term }
term        ::= factor { factor }
factor      ::= atom [ '*' | '+' | '?' ]
atom        ::= char | '(' regex ')'
char        ::= letter | digit | '_' | ...

----------------------------------------------------

regex     -> term regex_prime
regex_prime -> '|' term regex_prime | ε

term      -> factor term_prime
term_prime  -> factor term_prime | ε

factor    -> atom factor_prime
factor_prime -> '*' | '+' | '?' | ε

atom      -> char | '(' regex ')'

char      -> letter | digit | '_' | ... // 这里 ... 代表其他可能的字面字符