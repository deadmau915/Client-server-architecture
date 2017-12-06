import zmq
import random
import sys
import time
import json
import pygame
import math
import threading
from pygame.locals import *

#constantes
WIDTH = 1000
HIGH = 700

lock = threading.Lock()

position_list = [[310,50], [360,55], [410,65], [470,90], [525,130], [560,180], [590,230],
                 [605,290], [610,350], [605,400], [590,450], [570,500], [535,545], [500,580],
	             [450,610], [400,630], [355,640], [310,650], [260,640], [210,630], [170,610],
	             [130,580], [90,545], [60,500], [30,455], [20,395], [10,350], [15,300],
	             [25,255], [45,210], [70,165], [100,130], [135,105], [175,75], [215,60], [255,55]]

# set up the colors
BLACK = (  0,   0,   0)
WHITE = (255, 255, 255)
RED   = (255,   0,   0)
GREEN = (  0, 255,   0)
BLUE  = (  0,   0, 255)
COLOR_TEXT = (0, 128, 128)
RADIO = 300

class Node(pygame.sprite.Sprite):
	def __init__(self, id, img_path, coordX, coordY):
		pygame.sprite.Sprite.__init__(self)
		self.id = id
		self.ip = 'None'
		self.active = False
		self.image = pygame.image.load(img_path).convert_alpha()
		# self.image = pygame.image.load('nodes/node1.png').convert_alpha()
		self.rect = self.image.get_rect()
		self.coordX = coordX
		self.coordY = coordY
		self.keysRng = [self.id, -1]

def set_nodes(nodes_list):
	for i in xrange(1,37):
		path = 'nodes/node' + str(i) + '.png'
		n = Node(i, path, position_list[i-1][0], position_list[i-1][1])
		n.rect.x = n.coordX
		n.rect.y = n.coordY
		nodes_list.insert(i, n)

def set_background(screen):
	screen.fill(BLACK)
	titleFont = pygame.font.SysFont("comicsansms", 50)
	title = titleFont.render("*** Pirate Torrent ***", True, COLOR_TEXT)
	screen.blit(title,(WIDTH/2+130,15))
	pygame.draw.circle(screen, WHITE, (WIDTH/3, HIGH/2+20), 300, 3)
	pygame.display.flip()

def draw_nodes(nodes_list, screen):
	for node in nodes_list:
		if node.active:
			screen.blit(node.image, [node.coordX, node.coordY])
			pygame.display.flip()

def get_node_range(node):
	context_tmp = zmq.Context()
	tmp = context_tmp.socket(zmq.PUSH)
	tmp.connect(node.ip)
	req = {
		'source': 'terminal',
		'op': 'send keys range'
	}
	tmp.send(json.dumps(req))
	tmp.close()
	context_tmp.destroy()

def get_finger_table(nodes_list, node, finger_list):
	for x in xrange(1, 5):
		id = (node.id+pow(2,x))%36
		if nodes_list[id-1].active:
			finger_list.append(id)
		
def show_info(nodes_list, node, screen):
	subtitleFont = pygame.font.SysFont("comicsansms", 30)
	nodeInfo = subtitleFont.render("* Node Information *", True, COLOR_TEXT)
	fingerTbl = subtitleFont.render("* Finger Table *", True, COLOR_TEXT)
	screen.blit(nodeInfo, (WIDTH/2+110,80))
	screen.blit(fingerTbl, (WIDTH/2+330,80))

	textFont = pygame.font.SysFont("comicsansms", 20)
	node_id = "Node Id: " + str(node.id)
	nodeId = textFont.render(node_id, True, COLOR_TEXT)
	node_ip = "Node Ip: " + node.ip
	nodeIp = textFont.render(node_ip, True, COLOR_TEXT)
	keys_range = "Keys Range: [ " + str(node.keysRng[0]) + " - " + str(node.keysRng[1]) + " )"
	keysRng = textFont.render(keys_range, True, COLOR_TEXT)
	screen.blit(nodeIp,(WIDTH/2+125,115))
	screen.blit(keysRng,(WIDTH/2+125,145))
	screen.blit(nodeId,(WIDTH/2+125,175))

	finger_list = []
	get_finger_table(nodes_list, node, finger_list)
	cont = 0
	for x in finger_list:
		finger_id = "Node Id: " + str(x)
		fingerId = textFont.render(finger_id, True, COLOR_TEXT)
		screen.blit(fingerId,(WIDTH/2+345,115+(30*cont)))
		end_node = nodes_list[x-1]
		pygame.draw.line(screen, WHITE, [node.coordX, node.coordY], [end_node.coordX, end_node.coordY], 3)
		cont = cont + 1
	
	pygame.display.flip()

def check_click(nodes_list, x, y, screen):
	for node in nodes_list:
		if node.active:
			if node.rect.collidepoint(x, y):
				get_node_range(node)
				set_background(screen)
				# draw_nodes(nodes_list, screen)
				show_info(nodes_list, node, screen)
				draw_nodes(nodes_list, screen)

def handle_request(socket, nodes_list, screen):
	while True:
		req = socket.recv_json()
		if req['op']== 'add node':
			print "adding node"
			lock.acquire()
			nodes_list[int(req['my_id'])-1].active = True
			nodes_list[int(req['my_id'])-1].ip = req['my_ip']
			lock.release()
			draw_nodes(nodes_list, screen)
		elif req['op'] == 'delete node':
			print "deleting node"
			lock.acquire()
			nodes_list[int(req['my_id'])-1].active = False
			lock.release()
			set_background(screen)
			draw_nodes(nodes_list, screen)
		elif req['op'] == 'range':
			print "getting range"
			lock.acquire()
			nodes_list[int(req['my_id'])-1].keysRng[1] = int(req['rng'])
			lock.release()

def main():
	pygame.init()
	screen = pygame.display.set_mode([WIDTH,HIGH])
	pygame.display.set_caption("Pirate Torrent v5.5")
	reloj = pygame.time.Clock()

	if len(sys.argv) != 2:
		print 'Enter self ip addres'
		exit()

	context = zmq.Context()
	socket = context.socket(zmq.PULL)
	socket.bind('tcp://{}'.format(sys.argv[1]))

	nodes_list = []
	set_nodes(nodes_list)

	t = threading.Thread(target=handle_request, args=(socket, nodes_list, screen))
	t.start()

	set_background(screen)

	while True:
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				print "close window"
			elif event.type == pygame.MOUSEBUTTONDOWN:
				x, y = event.pos
				check_click(nodes_list, x, y, screen)

		reloj.tick(60)


if __name__ == '__main__':
	main()