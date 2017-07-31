#!/usr/bin/python
# -*- coding: utf-8 -*-

# ce script python enlève les bordures des images qui s'appellent Captude du ...

import Image
import os


def Traiter(fichier):
    img = Image.open(fichier)
    w, h = img.size
    img = img.crop((81,2,558,479))
    img.save('r'+fichier+'.jpg', quality=92)


Traiter("image.ppm")

