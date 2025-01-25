@main def calc() = {
 def gcd(a: Long, b: Long):Long=if (b==0) a else gcd(b, a%b)
 def lcm(a: Long, b: Long)=(a*b)/gcd(a,b)
 // run from 1 to 20
 var res = 1L;
 for ( i <- 2 to 20 ) {
   res = lcm(res, i)
 }
 println( res )
}