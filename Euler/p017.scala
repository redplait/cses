@main def calc() = {
 val single = Array[String]("zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
   "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen")
 val teens = Array[String]("", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety")
 def toEng(i: Int): String = {
   if ( i < 20 ) single(i)
   else if ( i < 100 ) teens(i / 10) + ( if (i % 10 != 0 ) single(i % 10) else "" )
   else if ( i < 1000 ) single(i / 100) + "hundred" ++ ( if (i % 100 != 0 ) "and" + toEng(i % 100) else "" )
   else toEng(i / 1000) + "thousand" ++ ( if (i % 1000 != 0 ) toEng(i % 1000) else "" )
 }
// println( toEng(42) )
 println( (1 to 1000).map( toEng(_).length ).sum )
}