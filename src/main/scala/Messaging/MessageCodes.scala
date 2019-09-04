package Messaging

object MessageCodes {


/*

  val code1=0
  val message1=""

*/


  val code_1 = -1
  val message_1="Pending status ..."

  val code_2 = -2
  val message_2="Global error ..."

  val code1 = 1
  val message1="Successfull ..."

  val code2=2
  val message2="Project Versions are invalid.\n" +
    "Possible reasons are: \n" +
    "1- Versions are discontinued: versions should be contineous 1,2,3,4,.... and not discontinued 1,3,6,7,... \n" +
    "2- You are trying to update a version N with a version which is not N+1 \n" +
    "( e.g. Always the new version should be an increment of lastest version) \n" +
    "3- You can run calculation on LatestVersion or create LatestVersion+1 \n" +
    "4- You cannot run calculation on version < LatestVersion"

  val code3=3
  val message3="Action keys should be only: add, update, delete."

  val code4=4
  val message4= "No DB Templates."

  val code5=5
  val message5="The Project you are calculating on does not exist or is not unique. The project needs to be created first."

  val code6=6
  val message6= "Cannot process data line {line}."

  val code7=7
  val message7= "Error in loading DB template."


  val code8=8
  val message8= "Process does not exist."

  val code9=9
  val message9= "Wrong dimensions for matrix A. Rows and columns fo A should ne equal."


  val code10=10
  val message10= "Wrong dimensions for matrix A and demand vector b. Rows of A should be equal to length of b."


  val code11=11
  val message11= "Wrong dimensions for matrix Q and demand vector lci. Cols of A should be equal to length of lci."



  val code12=12
  val message12= "Wrong dimensions for matrix B and scalars vector s. Cols of B should be equal to length of s."


  val code13=13
  val message13= "error parsing response json."

}
