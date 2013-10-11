#Grammar for betik#

```
<program>    := ( <function> | <statement> )*
<function>   := def <ident> ['(' [(<ident>,)*] ')'] [<block>] "end"
<block>      := <statement>*
<statement>  := <if_st> | <while_st> | return <expression> | print <expression> | <expression>
<if_st>      := if <expression> <block> [else <block>] end
<while_st>   := while <expression> <block> end
<expression> := (['('<expression>')'] | [<unary_op>] <value> [<binary_op>])*
<unary_op>   := '-'
<binary_op>  := '+' | '-' | '*' | '/' | '=' | '<>' | '>' | '<' | '>=' | '<=' | '==' | '.'
```
