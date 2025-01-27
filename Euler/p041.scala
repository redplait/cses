// sum from 1 to 9 is 45 - divisible by 3, so no pandigital prime can constist from 9 digits
// the same is true for 8 - sum from 1 to 8 is 36
// this gives us that pandigital primes can be 7 digits, max is (7654321) and sqrt of it is 2766.6
@main def calc() = {
 var pset = Array[Int](3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59,
  61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151,
  157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241,
  251, 257, 263, 269, 271, 277, 281, 283, 293)
 def isPrime(a: Int): Boolean = {
   pset.toList.forall( a % _ != 0 )
 }
 // fill primes from 297 to 2767
 for ( i <- 297 to 2767 by 2 ) if isPrime(i) then pset :+= i
 // filter our all even numbers and 5
 def fodd(l: Int): Boolean = {
   l == 1 || l == 3 || l == 7 || l == 9
 }
 def pow10(cum: Int, up: Int): Int = {
   up match {
     case 0 => cum
     case _ => pow10( 10 * cum, up - 1)
   }
 }
 def makePan(l: List[Int]): Int = {
   l.zipWithIndex.map( x => x._1 * pow10(1, x._2) ).sum
 }
 val res = for ( i <- List[Int](1,2,3,4,5,6,7).permutations if fodd(i.head) ) yield makePan(i)
 println( res.filter( isPrime(_) ).max )
}