```ebnf
-----------------------------------------------------------------------
# 核心结构 - 优先级从低到高：或 -> 连接 -> 量词 -> 原子

regex       ::=  term { '|' term }
              # 一个正则表达式是一个或多个 'term' 通过 '|' (或) 连接

term        ::=  factor { factor }
              # 一个 'term' 是一个或多个 'factor' 的连接 (隐式连接，例如 'ab' 表示 'a' 连接 'b')

factor      ::=  atom [ '*' | '+' | '?' ]
              # 一个 'factor' 是一个 'atom' 后面可选跟着一个量词 ('*', '+', '?')

atom        ::=  char_or_char_set | '(' regex ')'
              # 一个 'atom' 是一个单个字符/字符集，或者是一个用括号括起来的子正则表达式

# 字符和字符集定义 - 这是核心变化所在

char_or_char_set ::=  escaped_char | literal_char | char_set
                     # 一个字符或字符集可以是转义字符、字面字符，或者一个字符集定义

literal_char ::=  <任何字符，除了 '\', '(', ')', '*', '+', '?', '|', '[', ']'>
                # 字面字符是除了正则表达式元字符（本身有特殊含义）以外的任何字符。
                # 例如：'a', 'B', '0', '_', '=', '<', '!', '/', ';', ':', ',', '-' 等。

escaped_char ::=  '\' ( 's' | 'd' | 'w' |            # 字符类：空白、数字、单词字符
                       't' | 'n' | 'r' |             # 控制字符：制表、换行、回车
                       '+' | '*' | '?' |             # 转义元字符：加、星、问
                       '(' | ')' | '[' | ']' |       # 转义括号：左右圆括号、左右方括号
                       '|' | '\' )                   # 转义管道、转义反斜杠
                # 转义字符是反斜杠 '\' 后面跟着一个特定的字符，
                # 用于表示字符类、控制字符或元字符的字面含义。

char_set    ::=  '[' [ '^' ] (char_set_item { char_set_item }) ']'
                # 字符集以 '[' 开头，以 ']' 结尾。
                # 可选的 '^' 在 '[' 后面表示否定字符集 (匹配除了集合中所有字符以外的任何字符)。
                # 内部包含一个或多个 char_set_item。

char_set_item ::=  literal_char | escaped_char | range
                   # 字符集中的项可以是一个字面字符、一个转义字符，或者一个范围。

range ::= (digit_char | lowercase_letter | uppercase_letter) '-' (digit_char | lowercase_letter | uppercase_letter)
---------------------------------------------------------------------
```

```ebnf
// 1. 程序结构 (Program Structure)
// ---------------------------------------------------------------------

Program             ::=  FunctionDefinitionList EOF_TOKEN

FunctionDefinitionList ::=  FunctionDefinition FunctionDefinitionList
                        |  ε // 空，表示零个或多个函数定义

FunctionDefinition  ::=  TypeSpecifier Identifier '(' ParameterListOpt ')' CompoundStatement

ParameterListOpt    ::=  ParameterList
                        |  ε

ParameterList       ::=  TypeSpecifier Identifier ParameterListRest

ParameterListRest   ::=  ',' TypeSpecifier Identifier ParameterListRest
                        |  ε

// ---------------------------------------------------------------------
// 2. 语句 (Statements)
// ---------------------------------------------------------------------

Statement           ::=  DeclarationStatement
                        |  ExpressionStatement
                        |  CompoundStatement
                        |  SelectionStatement  // if-else
                        |  IterationStatement  // while
                        |  ReturnStatement
                        |  ';'                 // Empty statement

CompoundStatement   ::=  '{' StatementList '}'

StatementList       ::=  Statement StatementList
                        |  ε

DeclarationStatement ::=  TypeSpecifier Identifier OptionalInitializer ';'

OptionalInitializer ::=  '=' Expression
                        |  ε

ExpressionStatement ::=  OptionalExpression ';'

OptionalExpression  ::=  Expression
                        |  ε

// SelectionStatement 处理悬挂 else：优先匹配最近的 else
SelectionStatement  ::=  'if' '(' Expression ')' MatchedStatement
                        |  'if' '(' Expression ')' UnmatchedStatement

MatchedStatement    ::=  DeclarationStatement
                        |  ExpressionStatement
                        |  CompoundStatement
                        |  ReturnStatement
                        |  ';'
                        |  'if' '(' Expression ')' MatchedStatement 'else' MatchedStatement
                        |  'while' '(' Expression ')' MatchedStatement

UnmatchedStatement  ::=  'if' '(' Expression ')' Statement
                        |  'if' '(' Expression ')' MatchedStatement 'else' UnmatchedStatement
                        |  'while' '(' Expression ')' UnmatchedStatement


IterationStatement  ::=  'while' '(' Expression ')' Statement

ReturnStatement     ::=  'return' OptionalExpression ';'

// ---------------------------------------------------------------------
// 3. 表达式 (Expressions) - LR(1) 需要明确的优先级和结合性
//    优先级从低到高：Assignment -> Relational -> Additive -> Multiplicative -> Unary -> Postfix -> Primary
// ---------------------------------------------------------------------

Expression          ::=  AssignmentExpression

AssignmentExpression ::=  Identifier '=' AssignmentExpression // 右结合性
                        |  RelationalExpression

RelationalExpression ::=  AdditiveExpression RelationalOpRight

RelationalOpRight   ::=  ( '==' | '!=' | '<' | '<=' | '>' | '>=' ) AdditiveExpression RelationalOpRight
                        |  ε

AdditiveExpression  ::=  MultiplicativeExpression AdditiveOpRight

AdditiveOpRight     ::=  ( '+' | '-' ) MultiplicativeExpression AdditiveOpRight
                        |  ε

MultiplicativeExpression ::=  UnaryExpression MultiplicativeOpRight

MultiplicativeOpRight ::=  ( '*' | '/' ) UnaryExpression MultiplicativeOpRight
                        |  ε

UnaryExpression     ::=  ( '+' | '-' ) UnaryExpression // 一元正负
                        |  PostfixExpression

PostfixExpression   ::=  PrimaryExpression PostfixOpRight

PostfixOpRight      ::=  '(' ArgumentListOpt ')' PostfixOpRight // 函数调用
                        |  ε

ArgumentListOpt     ::=  ArgumentList
                        |  ε

ArgumentList        ::=  Expression ArgumentListRest

ArgumentListRest    ::=  ',' Expression ArgumentListRest
                        |  ε

PrimaryExpression   ::=  Identifier
                        |  IntegerConstant
                        |  '(' Expression ')'

// ---------------------------------------------------------------------
// 4. 终结符 (Terminals / Lexical Tokens)
//    (这些是词法分析器识别的最小单位，你需要在 TokenType 中定义它们)
// ---------------------------------------------------------------------

TypeSpecifier       ::=  INT // 对应 TokenType::INT
Identifier          ::=  // 对应 TokenType::IDENTIFIER
IntegerConstant     ::=  // 对应 TokenType::INTEGER
EOF_TOKEN           ::=  // 对应 TokenType::EOF_TOKEN
'+'                 ::=  // 对应 TokenType::PLUS
'-'                 ::=  // 对应 TokenType::MINUS
'*'                 ::=  // 对应 TokenType::MULTIPLY
'/'                 ::=  // 对应 TokenType::DIVIDE
'='                 ::=  // 对应 TokenType::ASSIGN
'=='                ::=  // 对应 TokenType::EQUAL
'!='                ::=  // 对应 TokenType::NOT_EQUAL
'<'                 ::=  // 对应 TokenType::LESS
'<='                ::=  // 对应 TokenType::LESS_EQUAL
'>'                 ::=  // 对应 TokenType::GREATER
'>='                ::=  // 对应 TokenType::GREATER_EQUAL
'('                 ::=  // 对应 TokenType::L_PAREN (需要新增 TokenType)
')'                 ::=  // 对应 TokenType::R_PAREN (需要新增 TokenType)
'{'                 ::=  // 对应 TokenType::L_BRACE (需要新增 TokenType)
'}'                 ::=  // 对应 TokenType::R_BRACE (需要新增 TokenType)
';'                 ::=  // 对应 TokenType::SEMICOLON (需要新增 TokenType)
','                 ::=  // 对应 TokenType::COMMA (需要新增 TokenType)
'if'                ::=  // 对应 TokenType::IF
'else'              ::=  // 对应 TokenType::ELSE
'while'             ::=  // 对应 TokenType::WHILE
'return'            ::=  // 对应 TokenType::RETURN
// 注意: `ε` 表示空串，不对应任何 Token
```