Test: 1, Object to string
Length: 43
Output: [ "apple", "orange", "pineapple", "pear" ]
Test passed

Test: 2, Custom object parse
[
Val(apple)
Val(orange)
Val(pineapple)
Val(pear)
]
Test passed

Test: 3, Object to string
Length: 28
Output: { "name":"dave", "id":345 }
Test passed

Test: 4, Object to string
Length: 33
Output: [ { "aaa":"bbb", "ccc":"ddd" } ]
Test passed

Test: 5, Object to string
Length: 20
Output: { "a":{ "b":"c" } }
Test passed

Test: 6, Object to string
Length: 33
Output: { "item":"computer", "id":1234 }
Test passed

Test: 7, Object to string
Length: 38
Output: { "a":{ "b":"c", "d":"e" }, "f":"g" }
Test passed

Test: 8, Object to string
Length: 41
Output: [ { "a":[ [ [ "b", { "c":"d" } ] ] ] } ]
Test passed

Test: 9, Object to string
Length: 72
Output: [ { "a":{ "b":"c", "d":"e" }, "f":"g", "h":{ "i":"j" }, "k":[ "l" ] } ]
Test passed

Test: 10, Attribute find, iterate over values
Find string(s)
Attribute string marmalade
Value string: orange
Value string: lemon
Value string: lime
Test passed

Test: 11, Object find, twice, same characteristics
Find string(s)
Find value string orange
Find value string ORAnge
Test passed

Test: 12, String to object, global buffer
Str: [ { "a":{ "b":-1.458e-20, "d":"e" }, "f":"g", "h":{ "i":"j" }, "k":[ "l" ] }, { "ff":"g" } ]
Size 366

Test: 13, Object finding
looked F, value 0
first find
Error: 0
Find ff, value 0
Error: 0
Find string tst, value 0

Test: 14, Object to string
Length: 94
Output: [ { "a":{ "b":-1.458e-20, "d":"e" }, "f":"g", "h":{ "i":"j" }, "k":[ "l" ] }, { "tst":"g" } ]
Test passed

Test: 15, IO put
Length: 92 92
Str: [ { "a":{ "b":-1.458e-20, "d":"e" }, "f":"g", "h":{ "i":"j" }, "k":[ "l" ] }, { "ff":"g" } ]]
Test passed


Test: 16, IO string replace

[0000]   [     {     "  a  "  :  {     "  b  "  :  -  1
[0010]   .  4  5  8  e  -  2  0  ,     "  d  "  :  "  e
[0020]   "     }  ,     "  f  "  :  "  g  "  ,     "  h
[0030]   "  :  {     "  i  "  :  "  j  "     }  ,     "
[0040]   k  "  :  [     "  l  "     ]     }  ,     {
[0050]   "  f  f  "  :  "  g  "     }     ]  ]
Len: 96
Str: [ { "a":{ "b":"cool foo bar", "d":"e" }, "f":"g", "h":{ "i":"j" }, "k":[ "l" ] }, { "ff":"g" } ]]

[0000]   [     {     "  a  "  :  {     "  b  "  :  "  c
[0010]   o  o  l     f  o  o     b  a  r  "  ,     "  d
[0020]   "  :  "  e  "     }  ,     "  f  "  :  "  g  "
[0030]   ,     "  h  "  :  {     "  i  "  :  "  j  "
[0040]   }  ,     "  k  "  :  [     "  l  "     ]     }
[0050]   ,     {     "  f  f  "  :  "  g  "     }     ]
[0060]   ]
Test passed

Test: 17, Simple string parse, empty attribute
Str: { "":0 }
Size 54
Test passed

Test: 18, String custom parse using own string buffer
Str: [ { "a":{ "b":-1.458e-20, "d":"e" }, "f":"g", "h":{ "i":"j" }, "k":[ "l" ] }, { "ff":"g" } ]
Size 366
Test passed

Test: 19, Simple string parse, uses callbacks
Str: [ { "a":{ "b":-1.458e-20, "d":"e" }, "f":"g", "h":{ "i":"j" }, "k":[ "l" ] }, { "ff":"g" } ]
Start

[
{
Prop(a)
{
Prop(b)
LVal(-1.458e-20)
Prop(d)
Val(e)
}
Prop(f)
Val(g)
Prop(h)
{
Prop(i)
Val(j)
}
Prop(k)
[
Val(l)
]
}
{
Prop(ff)
Val(g)
}
]
End

Test passed

Test: 20, Simple string parse, uses callbacks
Str: [ { "a":{ "b":-1.458e-20, "d":"e" }, "f":"g", "h":{ "i":"j" }, "k":[ "l" ] }, { "ff":"g" } ]
Start

[
{
Prop(a)
{
Prop(b)
LVal(-1.458e-20)
Prop(d)
Val(e)
}
Prop(f)
Val(g)
Prop(h)
{
Prop(i)
Val(j)
}
Prop(k)
[
Val(l)
]
}
{
Prop(ff)
Val(g)
}
]
End

Test passed

Test: 21, Simple string parse, uses callbacks
Str: [ { "aa":{ "bb":"", "dd":4 }, "f":"g", "h":{ "i":"j" }, "k":[ "l" ] }, { "ff":"g" } ]
Start

[
{
Prop(aa)
{
Prop(bb)
Val(", "dd":4 })
Prop(dd)
LVal(4)
}
Prop(f)
Val(g)
Prop(h)
{
Prop(i)
Val(j)
}
Prop(k)
[
Val(l)
]
}
{
Prop(ff)
Val(g)
}
]
End

Test passed

Test: 22, Attribute parse, uses callbacks and original string as buffer.
Str: [ { "a":{ "number":0, "test":"e" }, "f":"g", "h":{ "nothing":"something" }, "k":[ "l" ] }, { "ff":"g" } ]
Look for test.  Found: e
Look for number.  Found: 0
Look for something.  Didn't find anything
Test passed

Test: 23, Attribute parse, uses callbacks and original string as buffer, Replace both value and attribute.
Input: [{"start":{"number":0,"test":"string"},"fall":"good","FALL":["out","item"]},{"FALL2":"now"}]
Look for test.  Found: string
Look for fall (value), replaced with hello
Look for fall2 (value), replaced with hello2
Output: [{"start":{"number":0,"test":"string"},"fall":"hello","FALL":["out","item"]},{"FALL2":"hello2"}]
Look for fall (value).  Found: hello
Look for fall2 (attribute), renamed with stop
Output: [{"start":{"number":0,"test":"string"},"stop":"hello","FALL":["out","item"]},{"FALL2":"hello2"}]
Test passed

Test: 24, String parse, uses callbacks and original string as buffer.  Replace some strings.
Input: [ { "test":{ "routine":45, "offset":"byte" }, { "routine":"trap", "state":"go" }, "value1", "value2", "value3" ]
Look for offset, found at 35
Look for routine, replaced
Look for middle, replaced
Output: [ { "test":{ "routine":45, "offset":"byte" }, { "mouse":"trap", "state":"go" }, "value1", "middle", "value3" ]
Test passed

Test: 25, Simple string parse, bad JSON, unterminated value string
Str: [ "red", "green
Size 27
Last error: 5 at character 15
Test passed

Test: 26, Simple string parse, bad JSON, no ending array
Str: [ "brown", "blue"
Size 27
Last error: 1 at character 17
Test passed

Test: 27, Simple string parse, bad JSON, bad map
Str: [ "orange", { "yellow"
Size 33
Last error: 2 at character 22
Test passed

Test: 28, Simple string parse, bad JSON, bad map
Str: [ "orange2", { "yellow2" :
Size 54
Last error: 2 at character 26
Test passed

Test: 29, Simple string parse, bad JSON, bad map
Str: [ "orange3", { "yellow3" : 0
Size 54
Last error: 2 at character 28
Test passed

Test: 30, Simple string parse, bad JSON, bad array
Str: [ "orange4", { "yellow4" : 0 }
Size 81
Last error: 1 at character 30
Test passed

Press any key to continue . . .