#-*- coding: iso-8859-2 -*-
import pygame
#import pygame.font
#import pygame.image
pygame.init()
pygame.font.init()

fontname = "myfont"
pngName = fontname + ".png"
fontname = fontname + ".msh"

surf = pygame.image.load( pngName )

verts = ""
border = 2

gridSize = 32
halfGrid = gridSize / 2
halfGridF = halfGrid * 1.0

def getVert( iy, x1, x2, size ) :
    uvA = ( x1, iy * gridSize / 256.0)
    uvB = ( x2, iy * gridSize / 256.0)
    uvC = ( x1, ( iy + 1 )* gridSize / 256.0)
    uvD = ( x2, ( iy + 1 )* gridSize / 256.0)
    halfSize = size / 2.0
    sizef = size * 1.0
    posA = ( 0.0, halfGridF)
    posB = ( size, halfGridF)
    posC = ( 0.0,-halfGridF)
    posD = (  size,-halfGridF)
    out = ""
    out = out + str( posA[ 0 ] ) + " " + str( posA[ 1 ] ) + " " + str( uvA[ 0 ] ) + " " + str( uvA[ 1 ] ) + " \n"
    out = out + str( posB[ 0 ] ) + " " + str( posB[ 1 ] ) + " " + str( uvB[ 0 ] ) + " " + str( uvB[ 1 ] ) + " \n"
    out = out + str( posC[ 0 ] ) + " " + str( posC[ 1 ] ) + " " + str( uvC[ 0 ] ) + " " + str( uvC[ 1 ] ) + " \n"
    out = out + str( posD[ 0 ] ) + " " + str( posD[ 1 ] ) + " " + str( uvD[ 0 ] ) + " " + str( uvD[ 1 ] ) + " \n"
    out = out + str( size * 1.0) + " \n"
    return out
    
for iy in range( 8 ) :
    for ix in range(32) :
        xcoordBase = halfGrid + ix * gridSize
        ycoordBase = halfGrid + iy * gridSize
        index = ix + iy * 32
        sizeReady = False
        sizeX = 0
        
        
        
        
        while not sizeReady:
            rowFilled = False
            #testing only red
            for iiy in range( ycoordBase - 12, ycoordBase + 12 ):
                col1 = surf.get_at( (xcoordBase - sizeX, iiy ))
                col2 = surf.get_at( (xcoordBase + sizeX, iiy ))
                if col1[3]  * col1[0] / 256 > 128 or col2[ 3 ] * col2[0] / 256 > 128:
                    rowFilled = True
            if sizeX + 1 >= halfGrid or not rowFilled:
                sizeReady = True
            else:
                sizeX +=1
        
        sizeX = max( 2 * sizeX, 2 )
        print "char no {} done - sizX = {}".format(index, sizeX)
        size = (sizeX, gridSize)
        xcoord = xcoordBase - size[0] / 2
        ycoord = ycoordBase - size[1] / 2
        #if index == 32:
        #    verts = verts + getVert( iy, 0, gridSize / 1024.0, gridSize )
        #else:
        #    verts = verts + getVert( iy, xcoord / 1024.0, ( xcoord + size[ 0 ] ) / 1024.0, size[ 0 ] )
        verts = verts + getVert( iy, xcoord / 1024.0, ( xcoord + size[ 0 ] ) / 1024.0, size[ 0 ] )

open(fontname, "w").write(verts)