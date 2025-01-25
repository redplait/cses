@main def calc() = {
 // https://stackoverflow.com/questions/30280524/generate-prime-factors-of-a-number-in-scala
 def foo(x: Long, a: Int): List[Long] = if (a * a > x) List(x) else
    x % a match {
      case 0 => a :: foo(x / a, a)
      case _ => foo(x, a + 1)
    }
  println( foo(600851475143L, 2).reverse.head )
}