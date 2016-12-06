# pwrecov

This tool was primarily build to allow passwords or other secrets to be split among a set of users and only be recoverable if a the a predetermined limit of shareholders come together as well as an authorizer if needed.

This tool can be used to split and store passwords of kids/teenagers/anyones to the hands of shareholders in order to be able to recover the in case of an emergency. The key split into the number of shareholders (s) and requires a minumum threshold (t) of shareholders to combine their secret in order to recover the secret. For further trust from the secret owner, an authorizer can also be created. An authorizer owns a key that must be used even if every one of the shareholders comes together. The authorizer could be anyone from the police or an insuarance person to a trusted friend. Finally, to add more trust to the secret owner in the system, a destruction key can be created. The destruction key is owned by the secret owner but should be accessible to the shareholders in case of an emergency. 

The scheme looks like this:

generation
----------
secret --> destkey(secret) --> authorizer(destkey(secret))	--> share0(authorizer(destkey(secret)))
															--> share1(authorizer(destkey(secret)))
															--> share2(authorizer(destkey(secret)))
															--> ...
															--> sharen(authorizer(destkey(secret)))

recovery
--------
share0(authorizer(destkey(secret)))	--> authorizer(destkey(secret)) --> destkey(secret) --> secret
share1(authorizer(destkey(secret)))	-->
share2(authorizer(destkey(secret)))	-->
...								  	-->
share3(authorizer(destkey(secret)))	-->


When a destruction key is used, an AES key is generated and ecrypts the initial secret, if a destruction key is not used, the function does not perform an encryption an instead uses the same secret as the input to the next step in the pipeline.
When an authorizer is used, an AES key is generated and ecrypts the initial secret, if an authorizer is not used, the function does not perform an encryption an instead uses the same secret as the input to the next step in the pipeline.
The final stages uses shamirs secret sharing algorithm to split the current state of the secret into s shares and requires at least t shares in order to reconstruct the message and start the inversion of the pipeline.

Encryption Example:
pwrecov -g -a -d -s 3 -t 2
-g   : generate
-a   : use authorizer
-d   : use destructor key
-s 3 : create 3 shares
-t 2 : require 2 for decription

Decryption Example
pwrecov -r -a -d -s 3 -t 2 -v shares/share.simp.000 shares/share.simp.001 shares/share.simp.002 shares/share.auth.key shares/share.dest.key
-r   : recover
-a   : use authorizer
-d   : use destructor key
-s 3 : use 3 shares
-t 2 : use 2 for decription
-v   : verbose output
-f   : force the usage of the give parameters (if not used, file parameters used instead)
     : append all filenames to be used
