@main def calc() = {
 for ( a <- 1 to 1000;
       b <- a to 1000 if ( a + b ) < 1000
     )
 {
   val c = 1000 - a - b
   if ( a * a + b * b == c * c )
    println(a + " b" +  b + " c" + c + " prod " +  a * b * c)
 }
}