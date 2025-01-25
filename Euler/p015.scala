@main def calc() = {
 // from https://stackoverflow.com/questions/63664381/calculate-binomial-coefficient-for-large-n-in-scala
 def choose(n:Int,k:Int):BigInt = {
      if (k == 1)
        BigInt(n)
      else if (n == k)
        BigInt(1)
      else if (k == 0)
        BigInt(1)
      else
        choose(n-1, k-1) * n / k
    }
  println(choose(40, 20))
}