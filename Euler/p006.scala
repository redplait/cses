@main def calc() = {
 def sumN(n: Int): Long = {
   (n + 1) * n / 2
 }
 val sq = (1 to 100).fold(0)( (x, y) => x + y * y )
 val sumq = sumN(100)
 println( sumq * sumq - sq )
}