import scala.util.boundary, boundary.break

@main def calc() = {
 // from p44
 def isPent(i: Long): Boolean = {
   val s = Math.round(Math.sqrt(1 + 24 * i))
   s*s == 1 + 24 * i && (1 + s) % 6 == 0
 }
 // from p42
 def isTri(i: Long): Boolean = {
   val s = Math.round(Math.sqrt(1 + 8 * i))
   s*s == 1 + 8 * i && (-1 + s) % 2 == 0
 }
 boundary {
   for ( i <- 144L to 2000000000L ) {
     val hex = i * (2 * i - 1)
     if ( isPent(hex) && isTri(hex) ) {
       println(hex)
       break()
     }
   }
 }
}