@main def calc() = {
 def divCnt(a: Int): Int = {
   var res = 0;
   val end = Math.sqrt(a)
   for ( i <- 1 to Math.round(end.floatValue) ) {
     if ( a % i == 0 ) res = res + 2
   }
   // check for perfect square
   if ( end * end == a ) res = res + 1
   res
 }
 var tr = 0;
 // I hope our triangle has index < 100000
 for ( i <- 1 to 100000 ) {
   tr += i;
   if ( divCnt(tr) > 500 ) {
      println(tr)
      scala.util.control.Breaks.break
   }
 }
}