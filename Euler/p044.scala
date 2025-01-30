import scala.util.boundary, boundary.break

@main def calc() = {
 // idea stolen from https://martin-ueding.de/posts/project-euler-solution-44-pentagon-numbers/
 def isPent(i: Int): Boolean = {
   val s = Math.round(Math.sqrt(1 + 24 * i))
   s*s == 1 + 24 * i && (1 + s) % 6 == 0
 }
 // pentagonal numbers stream
 def pent(p: Int = 1, d: Int = 4): LazyList[Int] =
    LazyList.cons(p, pent(p + d, d + 3))
 boundary {
 for ( p_diff <- pent();
       pair <- pent().sliding(2).map( _ match {case Seq(x,y) => (x,y, x + p_diff)}).takeWhile( x => x._3 >= x._2 ) ) {
    val sum = pair._1 + pair._3
    if ( isPent(pair._3) && isPent(sum) ) {
      println(p_diff)
      break()
    }
  }
 }
}