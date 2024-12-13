object Match {

// from https://www.youtube.com/watch?v=dQ8ctYg3y8U
// each next level adds 4 for right-upper square +
// 2 matches * n - 1 for upper row +
// 2 matches * n - 1 for rigth column - so
// 2 * 2 * (n-1)
// to run: scala-cli run Match.scala -- N

def calc( n: Int ): Int = n match
 case 0 => 0
 case 1 => 4
 case _ => 4 + 4 * (n - 1) + calc( n - 1 )

def main(args: Array[String]): Unit = {
 val usage = """
    Usage: match size
  """
 if (args.length == 0) println(usage)
 else println( calc( args(0).toInt ) )
}
}