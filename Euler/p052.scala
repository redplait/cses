@main def calc() = {
 def isAnagram(l: Set[Int], j: Int): Boolean = {
  j.toString.toArray.map( _ - '0' ).forall( l.contains(_) )
 }
 def pow10(cum: Int, up: Int): Int = {
   up match {
     case 0 => cum
     case _ => pow10( 10 * cum, up - 1)
   }
 }
 for ( i <- ( 1 to 7 ) ) {
   val floor = pow10(1, i - 1)
   val ceil = pow10(1, i) / 6
   for ( j <- (floor to ceil) ) {
     val letters = j.toString.toArray.map( _ - '0' )
     val letSet = Set[Int](letters*)
     if ( (2 to 6).forall( x => isAnagram(letSet, x * j) ) ) then
       println(j)
   }
 }
}