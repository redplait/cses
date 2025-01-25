import scala.io.Source

@main def calc() = {
 // triangle presented as array of string, row 0 is root
 val source = Source.fromFile("0067_triangle.txt")
 val tr = source.getLines().toArray
 def parse(idx: Int): Array[Int] = {
   tr(idx).split(raw"\s").filter(_ != "").map(_.toInt).toArray
 }
 // traverse triangle from bottom - for each level we can make new array with max of previous level
 // left element has index i, right i + 1
 def levelUp(i: Int, prev: Array[Int]): Array[Int] = {
   var curr = parse(i)
   for ( idx <- 0 to curr.size - 1 ) curr(idx) += prev(idx).max(prev(idx+1))
   curr
 }
 // main
 source.close
 var old = parse(tr.size - 1)
 for ( i <- tr.size - 2 to 0 by -1 ) old = levelUp(i, old)
 println( old(0) )
}