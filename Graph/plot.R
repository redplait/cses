# simple test to plot CSES graphs using R package igraph
# based on tutorial https://kateto.net/netscix2016.html
library(igraph)
if ( file.exists("dedges.csv") ) {
 dir <- T
 fedges <- "dedges.csv"
} else {
 dir <- F
 fedges <- "edges.csv"
}
nodes <- read.csv("nodes.csv", header=T, as.is=T)
links <- read.csv(fedges, header=T, as.is=T)
net <- graph_from_data_frame(d=links, vertices=nodes, directed=dir)
# colors - 1 - just node, 2 - S, blue, 3 - T, red, 4 - condensed SCC, green
colrs <- c("gray50", "blue", "red", "green")
V(net)$color <- colrs[V(net)$type]
plot(net)
