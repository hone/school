///////////////////////////////////////////////////////////////////////////////
//
//    $Id: SealedDES.java,v 1.1 2008/09/10 20:21:47 randal Exp $
//
//    Randal C. Burns
//    Department of Computer Science
//    Johns Hopkins University
//
//    $Source: /home/randal/repository/public_html/420/src/SealedDES.java,v $
//    $Date: 2008/09/10 20:21:47 $        
//    $Revision: 1.1 $
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//  class: SealedDES
//
//  SealedDES encapsulates the DES encryption and decryption of Strings 
//  into SealedObjects.  It represesnts keys as integers (for simplicity).
//  
//  The main function gives and example of how to:
//    (1) generate a random 24 bit key by starting with a zero valued
//          8 bytes (64 bit key) and then encoding a string with that key
//    (2) perform a brute force search for that key and exame the 
//          resulting output for a known portion of plaintext (in this
//          case "Hopkins".
//
//  Your assignment will be to parallelize this process.
//
////////////////////////////////////////////////////////////////////////////////

import javax.crypto.*;
import java.security.*;
import javax.crypto.spec.*;

import java.util.Random;


class SealedDES
{
  // Cipher for the class
  Cipher des_cipher;

  // Key for the class
  SecretKeySpec the_key = null;

  // Constructor: initialize the cipher
  public SealedDES() 
  {
    try 
    {
      des_cipher = Cipher.getInstance("DES");
    } 
    catch ( Exception e )
    {
      System.out.println("Failed to create cipher.  Exception: " + e.toString() +
                           " Message: " + e.getMessage()) ; 
    }
  }

  // Decrypt the SealedObject
  //
  //   arguments: SealedObject that holds on encrypted String
  //   returns: plaintext String or null if a decryption error
  //     This function will often return null when using an incorrect key.
  //
  public String decrypt ( SealedObject cipherObj )
  {
    try 
    {
      return (String)cipherObj.getObject(the_key);
    }
    catch ( Exception e )
    {
//      System.out.println("Failed to decrypt message. " + ". Exception: " + e.toString()  + ". Message: " + e.getMessage()) ; 
    }
    return null;
  }

  // Encrypt the message
  //
  //  arguments: a String to be encrypted
  //  returns: a SealedObject containing the encrypted string
  //
  public SealedObject encrypt ( String plainstr )
  {
    try 
    {
      des_cipher.init ( Cipher.ENCRYPT_MODE, the_key );
      return new SealedObject( plainstr, des_cipher );
    }
    catch ( Exception e )
    {
      System.out.println("Failed to encrypt message. " + plainstr +
                         ". Exception: " + e.toString() + ". Message: " + e.getMessage()) ; 
    }
    return null;
  }

  // Set the key (convert from a long integer)
  public void setKey ( long theKey )
  {
    try 
    {
      // convert the integer to the 8 bytes required of keys
      byte[] deskey = new byte[8];
      deskey[0] = (byte)(theKey & 0xFF );
      deskey[1] =(byte)((theKey >> 8) & 0xFF );
      deskey[2] =(byte)((theKey >> 16) & 0xFF );
      deskey[3] =(byte)((theKey >> 24) & 0xFF );
      deskey[4] =(byte)((theKey >> 32) & 0xFF );
      deskey[5] =(byte)((theKey >> 40) & 0xFF );
      deskey[6] =(byte)((theKey >> 48) & 0xFF );
      deskey[7] =(byte)((theKey >> 56) & 0xFF );

      // Create the specific key for DES
      the_key = new SecretKeySpec ( deskey, "DES" );
    }
    catch ( Exception e )
    {
      System.out.println("Failed to assign key" +  theKey +
                         ". Exception: " + e.toString() + ". Message: " + e.getMessage()) ;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//
//  Revsion History 
//    
//  $Log: SealedDES.java,v $
//  Revision 1.1  2008/09/10 20:21:47  randal
//  Initial checkin.
//
//  Revision 1.1  2008/09/05 20:31:11  randal
//  Checkin.
//
//
////////////////////////////////////////////////////////////////////////////////
