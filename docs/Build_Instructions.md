Clone the repo:
``git clone https://gitlab.com/G_ka/sse-assets-optimiser.git``

Download Havok: 
https://drive.google.com/file/d/0B-85OBEi6SEkTmFxcEZtTHRxRm8/view?usp=sharing

Extract the "Source" and the "Lib" folders into ``CAO_backend/libs/havok_2010_2_0``

Download Havok Compatibility headers:
Clone this repo: https://github.com/figment/hkxcmd and grab the ``compat`` folder.

Put it into ``CAO_backend/libs/havok_2010_2_0``

``CAO_backend/libs/havok_2010_2_0`` should now contain three folders: ``Source``, ``Lib``, ``compat``.

Now, just open the CMake file with your IDE. You will have to generate a **32 bits** binary, 64 bits isn't supported.