
package org.ets.lca.Utilities

import java.util.concurrent._

import scala.collection.mutable
import scala.util.DynamicVariable

object Parallel {

  val forkJoinPool = new ForkJoinPool

  abstract class TaskScheduler {
    def schedule[T](body: => T): ForkJoinTask[T]
    def parallel[A, B](taskA: => A, taskB: => B): (A, B) = {
      val right = task {
        taskB
      }
      val left = taskA
      (left, right.join())
    }
  }

  class DefaultTaskScheduler extends TaskScheduler {
    def schedule[T](body: => T): ForkJoinTask[T] = {
      val t = new RecursiveTask[T] {
        def compute = body
      }
      forkJoinPool.execute(t)
      t
    }
  }

  val scheduler =
    new DynamicVariable[TaskScheduler](new DefaultTaskScheduler)

  def task[T](body: => T): ForkJoinTask[T] = {
    scheduler.value.schedule(body)
  }

  def parallel[A, B](taskA: => A, taskB: => B): (A, B) = {
    scheduler.value.parallel(taskA, taskB)
  }

  def parallel[A, B, C, D](taskA: => A, taskB: => B, taskC: => C, taskD: => D): (A, B, C, D) = {
    val ta = task { taskA }
    val tb = task { taskB }
    val tc = task { taskC }
    val td = taskD
    (ta.join(), tb.join(), tc.join(), td)
  }

  def parallelList[A](tasks: (() => A)*): Seq[A] = {
    if (tasks.isEmpty) Nil
    else {
      val pendingTasks = tasks.tail.map(t => task { t() })
      tasks.head() +: pendingTasks.map(_.join())
    }
  }



  def setup[A](SequenceLength:Int,
               func:((Int,Int)=>(() => mutable.MutableList[A])),parallelism:Int)={

//    println("parallelism:" + parallelism)
//    println("items:" + SequenceLength)

    var parallelFolds = parallelism

    val division = if (SequenceLength < parallelism) {
      parallelFolds = 1
      SequenceLength
    } else {
      SequenceLength / parallelism
    }

//    println("division:" + division)

    var iii = 0

    var tasks=mutable.MutableList[() => mutable.MutableList[A]]()

    while (iii < parallelFolds) {

      //println("Running: " + iii * division + "-->" + division * (iii + 1))
      tasks = tasks :+ func(iii * division, if (iii == parallelFolds - 1) SequenceLength else division * (iii + 1))

      iii = iii + 1

    }

    tasks
  }


  def parallelListUnit[Unit](tasks: (() => Unit)*): Seq[Unit] = {
    if (tasks.isEmpty) Nil
    else {
      val pendingTasks = tasks.tail.map(t => task { t() })
      tasks.head() +: pendingTasks.map(_.join())
    }
  }

}
