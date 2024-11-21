//http://aperiodic.net/phil/scala/s-99/
//Eight queens problem
//This is a classical problem in computer science.
// The objective is to place eight queens on a chessboard so that no two queens are attacking each other; i.e., no two
// queens are in the same row, the same column, or on the same diagonal.
//  Hint: Represent the positions of the queens as a list of numbers 1..N.
// Example: List(4, 2, 7, 3, 6, 8, 5, 1) means that the queen in the first column is in row 4,
// the queen in the second column is in row 2, etc. Use the generate-and-test paradigm.
//http://www.datagenetics.com/blog/august42012/

/**
  * Validates the last position
  */
def validate(queens: Seq[Int]): Boolean = {
  val last = queens.last
  val lastPos = queens.size - 1

  0 until lastPos map { pos =>
    val stepVal = queens(pos)

    // not on the same row , or
    // not on the same diagonal (if the distance between fields is the same as the distance between columns)
    stepVal == last || Math.abs(lastPos - pos) == Math.abs(last - stepVal)
  } forall( _ == false)
}

def generateSteps(queens: Seq[Int] = Seq.empty[Int], step: Int = 0, tableSize: Int = 8): Seq[Seq[Int]] = {
  if (step == tableSize) {
    Seq(queens)
  } else {
    1 to tableSize map (queens :+ _) filter (validate) flatMap (s => generateSteps(s, step + 1, tableSize))
  }
}

def main(args: Array[String]): Unit = {
 val solution = generateSteps()
 println(solution.size)
 for ( n <- solution ) println(n)
}