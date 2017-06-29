#-*- coding: iso-8859-2 -*-
import pygame
#import pygame.font
#import pygame.image
pygame.init()
pygame.font.init()

font = pygame.font.SysFont("Arial", 18)
#font2 = pygame.font.SysFont("SegoeScript", 22)
#font3 = pygame.font.SysFont("SegoeScript", 26)
#font2 = pygame.font.SysFont("arial", 18)
#font3 = pygame.font.SysFont("comicsansms", 20)
surf = pygame.Surface( (1024, 256 ))
surf.fill((0, 0, 0))
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
        try:
            
            
            textRect = font.render( chr( index ), True, (255, 255, 255) )
            size = textRect.get_size()
            
            xcoord = xcoordBase - size[0] / 2
            ycoord = ycoordBase - size[1] / 2
            
            surf.blit( textRect, (xcoord, ycoord) )
            
           
            #textRect = font3.render( chr( index ), True, (255, 255, 255) )
            #size = textRect.get_size()
            #xcoord = halfGrid + ix * gridSize - size[0] / 2
            #ycoord = halfGrid + iy * gridSize - size[1] / 2
            #surf.blit( textRect, (xcoord, ycoord) )

            #textRect = font2.render( chr( index ), True, (255, 255, 255) )
            #size = textRect.get_size()
            #xcoord = halfGrid + ix * gridSize - size[0] / 2
            #ycoord = halfGrid + iy * gridSize - size[1] / 2
            #surf.blit( textRect, (xcoord, ycoord) )
            pygame.draw.rect(surf, (255,0,0 ), (xcoordBase - 10, ycoordBase - 10, 32, 32), 1)
            
            verts = verts + getVert( iy, xcoord / 1024.0, ( xcoord + size[ 0 ] ) / 1024.0, size[ 0 ] )
        except pygame.error:
            if index == 32:
                verts = verts + getVert( iy, 0, gridSize / 1024.0, gridSize )
            else :
                uvs = verts = verts + getVert( iy, 0, 0, 0 )
            pass
pygame.image.save( surf, "font.png" )
open('font.msh', "w").write(verts)