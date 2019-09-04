package Messaging

import org.ets.lca.PlayGround

/**
  * Created by francoissaab on 1/10/18.
  */
class MessagingUtils {

  def toJSON(message:Message)={

    "{'code':"+message.Code+"' , 'message':"+message.Message+"'}"

  }

  def throwMessageAsException(message:Message)={


    PlayGround.message=message

    throw new CustomException(message)



  }

}
