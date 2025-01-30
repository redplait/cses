@main def calc() = {
 var pset = Array[Int](3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59,
  61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151,
  157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241,
  251, 257, 263, 269, 271, 277, 281, 283, 293)
 def isPrime(a: Int): Boolean = {
   pset.toList.forall( a % _ != 0 )
 }
 // fill primes from 297 to 9999
 for ( i <- 297 to 9999 by 2 ) if isPrime(i) then pset :+= i
 // store them in set
 val primes = Set[Int]( pset* );
 // filter our all even numbers and 5
 def fodd(l: Int): Boolean = {
   l == 1 || l == 3 || l == 7 || l == 9
 }
 def makePan(l: Array[Int]): Int = {
   l(3) + l(2) * 10 + l(1) * 100 + l(0) * 1000
 }
 def isAnagram(l: Set[Int], j: Int): Boolean = {
  j.toString.toArray.map( _ - '0' ).forall( l.contains(_) )
 }
 for ( i <- pset if ( i > 1000 && i < 5000 ) ) {
   val letters = i.toString.toArray.map( _ - '0' )
   val letSet = Set[Int](letters*)
   val res = for ( l <- letters.permutations if fodd(l(3)) ) yield makePan(l)
   for ( j <- res if primes.contains(j) && j != i ) {
      val diff = j - i;
      if ( diff > 0 ) {
        val third = j + diff
        if ( primes.contains(third) && isAnagram(letSet, third) )
          println("> " + i + " " + j + " " + third)
     }
   }
 }
}