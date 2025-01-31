import scala.util.boundary, boundary.break

@main def calc() = {
 boundary {
  val sf = new Iterator[BigInt] {
    var n1:BigInt = 1
    var n2:BigInt = 1
    def hasNext = true
    def next(): BigInt = { val res = n1 + n2; n2 = n1; n1 = res; res }
  }
  for ( i <- (3 to 10000) ) {
   val j = sf.next
   if ( j.toString.length >= 1000 ) { println(i); break() }
  }
 }
}