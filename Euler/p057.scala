@main def calc() = {
 def gcd(a: BigInt, b: BigInt):BigInt=if (b==0) a else gcd(b, a%b)
 // lazy list of fractions
 // n' = n + 2d, d' = n + d
 def frac(n: BigInt = 3, d: BigInt = 2): LazyList[(BigInt, BigInt)] = {
    val dd = n + d
    val nn = n + 2 * d
    val g = gcd(nn, dd)
    LazyList.cons((n, d), frac( nn / g, dd / g))
 }
 println( (1 to 1000).map( frac() ).filter( x => x._1.toString.length > x._2.toString.length ).size )
}