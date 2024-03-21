## Building
To compile yourself you need ```windres``` and ```gcc``` from mingw or something.

I suggest you edit the path to your Gremlins_Inc.exe in the main.c

Run ```.\make.bat```

When after some time Gremlins will update you will need to patch the Assembly-CSharp.dll manually again.

Assembly-CSharp.dll is located in the ```Steam\steamapps\common\Gremlins\Gremlins_Inc_Data\Managed\Assembly-CSharp.dll```

To achieve what I have, you'll need to patch CCreateGameForm.ShowForm():

![image](https://github.com/forentfraps/Gremlins-Password/assets/29946764/c3ea5e8d-3874-4f61-a2d2-4923730e1c93)

and also patch CSessionPasswordForm.OnJoinClick():

![image](https://github.com/forentfraps/Gremlins-Password/assets/29946764/3f44820f-4cbf-4d8c-a78c-d58b034f59c3)


## Using 

Replace the dll.

Start the Injector in the same dir as the dll.

Start Gremlins_Inc from steam
