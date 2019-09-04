name := "UBUUBI-LCA-Calculator"

version := "1.0"

scalaVersion := "2.11.8"

libraryDependencies ++= Seq(

  "net.sf.trove4j" % "trove4j" % "3.0.1",
"org.slf4j" % "slf4j-api" % "1.7.21"
)

fork in run := true

javaOptions in run ++= Seq(
  "-Xms4G", "-Xmx8G", "-XX:MaxPermSize=1024M", "-XX:+UseConcMarkSweepGC")
