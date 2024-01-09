#!Rscript
# some experiments with package Ckmeans.1d.dp to try solve CSES task Houses and Schools
library(Ckmeans.1d.dp)

# globals: g_N, g_K & array g_A
g_N <- 0
g_K <- 0
g_A <- c()
# read data
read_data = function(fh) {
  n1 <- unlist(strsplit(readLines(fh, n=1), " "))
  g_N <<- as.numeric(n1[1])
  g_K <<- as.numeric(n1[2])
  # read array
  n1 <- unlist(strsplit(readLines(fh, n=1)," "))
  g_A <<- c(as.numeric(n1))
}

# main
# read command line arg
args <- commandArgs(trailingOnly=TRUE)
if (length(args)==0) {
 # read from stdin
 read_data(stdin())
} else {
 # read from args[1]
 if ( !file.exists(args[1]) )
 {
   stop("input file don`t exists", call.=FALSE)
 }
 fh <- file(args[1], "r")
 read_data(fh)
 close(fh)
}
# main call
res <- Ckmeans.1d.dp(x = g_A, k = g_K, y = 1)
# dump
print(res$centers)

