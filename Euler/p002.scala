@main def calc() = {
 // from https://www.baeldung.com/scala/generating-fibonacci-series
 lazy val fib: LazyList[Long] = 0L #:: 1L #:: fib.zip(fib.tail).map(_ + _)
 println( fib.takeWhile(_ < 4000000 ).filter( _ % 2 == 0).foldLeft(0L)(_ + _) )
}