import collection.immutable.Set

@main def calc() = {
 var pset = Array[Int](2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59,
  61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151,
  157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241,
  251, 257, 263, 269, 271, 277, 281, 283, 293)
 def isPrime(a: Int): Boolean = {
   pset.toList.forall( a % _ != 0 )
 }
 // fill primes from 297 to 999999
 for ( i <- 297 to 999997 by 2 ) if isPrime(i) then pset :+= i
// primes 78498 last 999983
//println("primes " + pset.size + " last " + pset(pset.size - 1))
 var longest = 1
 var lmax = 1
 val primes = Set[Int]( pset* )
 def conSum(from: Int) = {
  var roll = pset(from)
  for ( i <- from + 1 to pset.size - 1 if roll < 1000000 ) {
    roll += pset(i)
    val diff = i - from
    if ( diff > longest && primes.contains(roll) ) {
      longest = diff; lmax = roll
    }
  }
 }
 for ( i <- 0 to pset.size - 1 ) conSum(i)
 println( lmax )
 println( longest )
}