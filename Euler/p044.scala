import scala.util.boundary, boundary.break

@main def calc() = {
 object AllDone extends Exception { }
 // idea stolen from https://martin-ueding.de/posts/project-euler-solution-44-pentagon-numbers/
 def isPent(i: Int): Boolean = {
   val s = Math.round(Math.sqrt(1 + 24 * i))
   s*s == 1 + 24 * i && (1 + s) % 6 == 0
 }
 // pentagonal numbers stream
 def pent(p: Int = 1, d: Int = 4): LazyList[Int] =
    LazyList.cons(p, pent(p + d, d + 3))
 try {
 for ( p_diff <- pent() ) boundary {
   for ( pair <- pent().sliding(2).map{case Seq(x,y) => (x,y)} ) {
    val upper = pair._1 + p_diff
    if ( upper < pair._2 ) then break()
    val sum = pair._1 + upper
    if ( isPent(upper) && isPent(sum) ) {
      println(p_diff)
      throw AllDone
    }
   }
  }
 } catch {
   case AllDone =>
 }
}