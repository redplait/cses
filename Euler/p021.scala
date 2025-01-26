@main def calc() = {
  var kach = collection.mutable.Map.empty[Int, Int]
  def divSum(n: Int): Int = {
    kach.get(n) match {
       case Some(v) => v
       case None => val res = (1 to n - 1).filter(n % _ == 0).sum; kach.addOne(n, res); res
    }
  }
  def isAmic(n: Int): Boolean = {
    val sm = divSum(n)
    sm != n && n == divSum(sm)
  }
  println( (1 to 10000 - 1).filter( isAmic(_) ).sum )
}