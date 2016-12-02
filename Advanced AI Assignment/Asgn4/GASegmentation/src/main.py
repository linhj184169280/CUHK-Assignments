'''
Created on 2016/5/3

@author: linhuangjing
'''
import numpy as np
import scipy.ndimage as af
from scipy import misc
import random
import matplotlib.pyplot as plt
from skimage import feature

class GASegmenter:
    def __init__(self):
        '''
        Constructor
        '''
        self._e = 2.71
        self._radius = 3.0
        self._mutateProb = 1.0/250
        self._delta_in_W = 0.3
        self._population_num_local = 10
        self._population_num = 5
        self._unchange_stop_thres_local = 10
        self._unchange_stop_thres = 1000
        self._WNCut_stop_thres = 1.0    
        self._Optimal_Prob = 0.2
        self._alpha_Dis_Val = 0.05
        
    def _computeW(self, img):
        e = self._e
        delta = self._delta_in_W
        param_a = 0.0
        img_Size = img.shape[0] * img.shape[1]
        W = np.zeros( [img_Size,img_Size], dtype = "float32" )
        print "Generating W ..."
        prog = prog_t = 0
        overall = W.shape[0]
        interval = overall/10
        for i in xrange(W.shape[0]):
            # Computing progress complementation...
            prog = prog + 1
            prog_t = prog_t + 1
            if prog_t > interval:
                print "W Complete %.2f %%..." %(float(prog)*100/overall)
                prog_t = 0
                
            p1_x = i % img.shape[1]
            p1_y = i / img.shape[1]
            p2_x_m = int(p1_x - self._radius)
            p2_x_M = int(p1_x + self._radius)
            p2_y_m = int(p1_y - self._radius)
            p2_y_M = int(p1_y + self._radius)
            if p2_x_m < 0:   # in case of outing the boundary
                p2_x_m = 0
            if p2_x_M > img.shape[1]:
                p2_x_M = img.shape[1]
            if p2_y_m < 0:
                p2_y_m = 0
            if p2_y_M > img.shape[0]:
                p2_y_M = img.shape[0]
        
            for p2_y in xrange(p2_y_m, p2_y_M):
                for p2_x in xrange(p2_x_m, p2_x_M):
                    if ((p1_x-p2_x) ** 2 + (p1_y-p2_y) ** 2) >= self._radius ** 2 or (p1_x==p2_x and p1_y==p2_y):
                        continue
                    else:
                        if p1_x-p2_x != 0: # Make sure k != 0
                            k = float(p1_y-p2_y)/float(p1_x-p2_x)
                            b = float(p1_x*p2_y-p1_y*p2_x)/float(p1_x-p2_x)
                            func = lambda X: k * X + b        
                            if p1_x < p2_x:
                                m = p1_x
                                M = p2_x
                            else:
                                m = p2_x
                                M = p1_x
                            max_on_line = 0.0
                            x = m
                            X = m+1
                            while X <= M :
                                y = int(func(x) + 0.5)
                                Y = int(func(X) + 0.5)
                                grad = abs(img[Y,X]-img[y,x])
                                if grad > max_on_line:
                                    max_on_line = grad
                                    if grad > param_a:
                                        param_a = grad
                                x = x + 1
                                X = X + 1
                            j = int(p2_y) * img.shape[1] + int(p2_x)
                            W[i, j] = ( -float(max_on_line)**2 )
                            if W[i, j] < 0.1 ** 10 and W[i, j] > -0.1 ** 10:
                                W[i, j] = -0.1 ** 10
                            #print "here: p1(%d, %d), p2(%d, %d), W = %f" %(p1_x, p1_y, p2_x, p2_y, W[i, j])
                        else:
                            x = X = p1_x
                            if p1_y < p2_y:
                                m = p1_y
                                M = p2_y
                            else:
                                m = p2_y
                                M = p1_y
                            max_on_line = 0.0
                            y = m
                            Y = m+1
                            while Y<=M:
                                grad = abs(img[Y,X]-img[y,x])
                                if grad > max_on_line:
                                    max_on_line = grad
                                    if grad > param_a:
                                        param_a = grad
                                y = y + 1
                                Y = Y + 1
                            j = int(p2_y) * img.shape[1] + int(p2_x)
                            W[i, j] = ( -float(max_on_line)**2 )
                            if W[i, j] < 0.1 ** 10 and W[i, j] > -0.1 ** 10:
                                W[i, j] = -0.1 ** 10
                            #print "here: p1(%d, %d), p2(%d, %d), W = %f" %(p1_x, p1_y, p2_x, p2_y, W[i, j])
                            
        W = W / (2 * delta * param_a**2)
        t_W = np.zeros( [img_Size,img_Size], dtype = "float32" )
        t_W[W<0] = 1
        W = (e ** W) * t_W           
        misc.imsave(imgDir + "W.jpg", W)
        print "Finish W computing"
        return W
    
    
    def _Get_h_Neighbor(self, W, index, h):
        t_nei = W[:, index]
        weight_now = -1.0
        nei_index_list = (-t_nei).argsort()
        h_nei_list = []
        h_t =0
        for i in xrange(nei_index_list.shape[0]):
            if t_nei[nei_index_list[i]] == 0:  #if weight is 0.0 then stop calculation
                break
            if weight_now != t_nei[nei_index_list[i]]: #a new weight considered, the flag h_t should be modified
                weight_now = t_nei[nei_index_list[i]]
                h_nei_list.append(nei_index_list[i])
                h_t = h_t + 1
            else:
                h_nei_list.append(nei_index_list[i])
            if h_t > h:
                break
        
        return h_nei_list[ random.randint( 0, len(h_nei_list)-1) ]
            
    
    def _initialize(self, W, num_popu = 10):
        population = []
        for i in xrange(num_popu):
            individual = []
            for j in xrange(W.shape[0]):
                nei = self._Get_h_Neighbor(W, j, 2)              
                individual.append(nei)
            population.append(np.array(individual))
        return population
    
    def _groupObjs(self, individual):
        '''Group objects of individual and return results as a list
        '''
        objects = [] #result
        def _isInObjects(objects, node):
            '''Judge whether node is in the objects
               if is in, return the position,else return -1
            '''
            pos = -1
            for i in xrange(len(objects)):
                if node in objects[i]:
                    pos = i
                    break
            return pos

        for i in xrange(len(individual)):
            pos1 = _isInObjects(objects, i)
            pos2 = _isInObjects(objects, individual[i])
            if pos1 == -1 and pos2 == -1:
                objects.append([i, individual[i]])
                continue
            if pos1 != -1 and pos2 == -1:
                objects[pos1].append(individual[i]) 
                continue
            if pos1 == -1 and pos2 != -1:
                objects[pos2].append(i)                               
                continue
            if pos1 != -1 and pos2 != -1:
                if pos1 == pos2:
                    continue                
                objects[pos1].extend(objects[pos2])                
                del objects[pos2]           
        
        return objects
    
    def _Objs2Image(self, objects, height, width):
        '''label different objects in random colors, return the labelled image as result
        '''
        image = np.zeros([height,width,3])
        for i in xrange(len(objects)):
            r = random.randint(0,255)
            g = random.randint(0,255)
            b = random.randint(0,255)
            for j in xrange(len(objects[i])):
                p_x = objects[i][j] % width
                p_y = objects[i][j] / width
                image[p_y, p_x, 0] = r
                image[p_y, p_x, 1] = g
                image[p_y, p_x, 2] = b
        return image
    
    def _evalFunction(self, objects, W):
        WNCut = 0
        if len(objects) == 1:
            return 10 ** 8
        for k in xrange(len(objects)):
            Cr = 0.0
            Mr = 0.0
            M = W.sum()/2
            W_t = np.array(W)
            for m in xrange(len(objects[k])):
                for n in xrange(len(objects[k])):
                    p1 = objects[k][m]
                    p2 = objects[k][n]
                    W_t[p1,p2] = 0
            Cr = W_t.sum()/2
            for m in xrange(len(objects[k])):
                for n in xrange(len(objects[k])):
                    p1 = objects[k][m]
                    p2 = objects[k][n]
                    Mr = Mr + W[p1, p2]
            Mr = Mr / 2      
            cutA_AV = Cr
            assocA_V = Mr + Cr
            assocVA_V = (M - Mr) + Cr
            WNCut = WNCut + (cutA_AV/assocA_V + cutA_AV/assocVA_V )  #+ cutA_AV/assocVA_V                
            #WNCut = WNCut + Cr/Mr
        return WNCut
    
    def _GAEvolution(self, W, population, objects_list, WNCut_list, mutate_prob, localflag = True):
        if random.random() < self._Optimal_Prob:
            index1 = index2 = WNCut_list.index(min(WNCut_list))
        else:
            index1 = index2 = random.randint(0,len(population)-1)
        while index1 == index2:
            index2 = random.randint(0,len(population)-1)
        individual1 = population[index1]
        individual2 = population[index2]
        mask = (np.random.random(individual1.shape[0]) + 0.5).astype('int')
        #Crossover
        new_individual = mask * individual1 + (1-mask) *individual2
        
        
        if random.random() < 0.03:
            t_individual = []
            for j in xrange(W.shape[0]):
                nei = self._Get_h_Neighbor(W, j, 2)              
                t_individual.append(nei)
            new_individual = np.array(t_individual)
                
        #Mutation
        for i in xrange(new_individual.shape[0]):
            if random.random() < mutate_prob:
                nei = self._Get_h_Neighbor(W, i, 2)
                new_individual[i] = nei
        new_objects = self._groupObjs(new_individual)
        new_WNCut = self._evalFunction(new_objects, W)
        #max_WNCut = max(WNCut_list)
        #max_index = WNCut_list.index( max_WNCut )
             
        is_in_population = False
        for i in xrange(len(population)):
            if (new_individual-population[i]).any() == False:
                print "Same individual"
                is_in_population = True
            
        if is_in_population == False:
            #insert new individual into population
            population.append(new_individual)
            objects_list.append(new_objects)
            WNCut_list.append(new_WNCut)  
            
            #kill the worst individual
            #if min(WNCut_list) < 7.0 and max(WNCut_list) < 7.0*1.5:
            if localflag == True:
                del_index = WNCut_list.index(max(WNCut_list))
                del population[del_index]
                del objects_list[del_index]
                del WNCut_list[del_index]
                
            else:
            
            
            #kill the worsest individual 
                Value_list = []
                best_index = WNCut_list.index(min(WNCut_list))
                max_dis_diff = 0.0
                max_val_diff = 0.0
                for i in xrange(len(population)):
                    if i != best_index:
                        arr = abs(population[i]-population[best_index])
                        arr[arr>0] = 1
                        arr.sum()
                        if arr.sum() > max_dis_diff:
                            max_dis_diff = float(arr.sum())
                        if WNCut_list[i]-WNCut_list[best_index] > max_val_diff:
                            max_val_diff = float(WNCut_list[i]-WNCut_list[best_index])
                for i in xrange(len(population)):
                    if i != best_index:
                        arr = abs(population[i]-population[best_index])
                        arr[arr>0] = 1
                        arr.sum()
                        if WNCut_list[i] == WNCut_list[best_index] and arr.sum()== 0:
                            t_value = 100000
                        else:
                            t_value = self._alpha_Dis_Val * self._e**(float(arr.sum())/900) + (1 - self._alpha_Dis_Val)*(self._e**float(-WNCut_list[i]+WNCut_list[best_index]))
                        Value_list.append(t_value)
                    else:
                        Value_list.append(1.0)    
                
                del_index = Value_list.index( min(Value_list) )
                del population[del_index]
                del objects_list[del_index]
                del WNCut_list[del_index]
            
        return population, objects_list, WNCut_list
    
    def GenerateGoodSolution(self, W, img):
        population = self._initialize(W, num_popu = self._population_num_local)
        objects_list = []
        WNCut_list = []
        for i in xrange(len(population)):    
            objects = self._groupObjs(population[i])
            WNCut = self._evalFunction(objects, W)
            objects_list.append(objects)
            WNCut_list.append(WNCut)
        print "min WNCut %f" %(min(WNCut_list))
        
        iter_num = 0
        unchangeFlag = False
        unchangeNum = 0
        t_min = t_max = 0
        while min(WNCut_list) > self._WNCut_stop_thres and unchangeFlag == False:
        #for i in xrange(20):
            population, objects_list, WNCut_list = self._GAEvolution(W, population, objects_list, WNCut_list, self._mutateProb, localflag = True)  
            self._X_iteration.append(self._iteration)
            self._Y_minWNCut.append(min(WNCut_list))
            self._Y_maxWNCut.append(max(WNCut_list))
            print "%d iteration, the min WNCut = %f, max WNCut = %f, object_num = %d" %(iter_num, min(WNCut_list), max(WNCut_list), len(objects_list[WNCut_list.index(min(WNCut_list) )]))
            self._iteration = self._iteration + 1
            iter_num = iter_num + 1
            
            # Stopping criterion: 300 iterations unchanged, then stop it...
            if t_min != min(WNCut_list): #or t_max != max(WNCut_list):
                t_min = min(WNCut_list)
                t_max = max(WNCut_list)
                unchangeNum = 0
            else:
                unchangeNum = unchangeNum + 1
            #if unchangeNum > 75:
            #    mutate_prob = mutate_prob/2
            if unchangeNum > self._unchange_stop_thres_local:
                unchangeFlag = True
            
            if iter_num % 100 == 0:
                index = WNCut_list.index(min(WNCut_list) )
                objects = objects_list[index]
                img_label = self._Objs2Image(objects, img.shape[0], img.shape[1])
                misc.imsave(imgDir + "Demo1_label.jpg", img_label)
                WNCut = self._evalFunction(objects, W)
        
        best_index = WNCut_list.index(min(WNCut_list))
        return  population[best_index]
        
        
    
    def segment(self, img):
        #Calculate W
        W = self._computeW(img)
        population = []
        objects_list = []
        WNCut_list = []
        
        self._iteration = 0
        self._X_iteration = []
        self._Y_minWNCut = []
        self._Y_maxWNCut = []
        
        for i in xrange(self._population_num):
        #Initialize population
            population.append( self.GenerateGoodSolution( W, img) )
            
        for i in xrange(len(population)):    
            objects = self._groupObjs(population[i])
            WNCut = self._evalFunction(objects, W)
            objects_list.append(objects)
            WNCut_list.append(WNCut) 
        
        print "min WNCut %f" %(min(WNCut_list))
        
        iter_num = 0
        unchangeFlag = False
        unchangeNum = 0
        t_min = t_max = 0
        while min(WNCut_list) > self._WNCut_stop_thres and unchangeFlag == False:
        #for i in xrange(20):
            population, objects_list, WNCut_list = self._GAEvolution(W, population, objects_list, WNCut_list, self._mutateProb, localflag = True)  
            self._X_iteration.append(self._iteration)
            self._Y_minWNCut.append(min(WNCut_list))
            self._Y_maxWNCut.append(max(WNCut_list))
            print "%d iteration, the min WNCut = %f, max WNCut = %f, object_num = %d" %(iter_num, min(WNCut_list), max(WNCut_list), len(objects_list[WNCut_list.index(min(WNCut_list) )]))
            self._iteration = self._iteration + 1
            iter_num = iter_num + 1
            
            # Stopping criterion: 300 iterations unchanged, then stop it...
            if t_min != min(WNCut_list) or t_max != max(WNCut_list):
                t_min = min(WNCut_list)
                t_max = max(WNCut_list)
                unchangeNum = 0
            else:
                unchangeNum = unchangeNum + 1
            #if unchangeNum > 75:
            #    mutate_prob = mutate_prob/2
            if unchangeNum > self._unchange_stop_thres:
                unchangeFlag = True
            
            if iter_num % 100 == 0:
                index = WNCut_list.index(min(WNCut_list) )
                objects = objects_list[index]
                img_label = self._Objs2Image(objects, img.shape[0], img.shape[1])
                misc.imsave(imgDir + "Demo1_label.jpg", img_label)
                WNCut = self._evalFunction(objects, W)
            
            
        plt.plot(self._X_iteration, self._Y_minWNCut, 'r')
#        plt.plot(X_iteration, Y_maxWNCut, 'b')
        plt.xlabel('iterations')
        plt.ylabel('Value of WNCut (red->min, blue->max)')
        plt.savefig( imgDir+ "Demo1_Figure.png" )

        result = img_label
        return result
        

if __name__ == '__main__':
    #imgDir = "D:/ScreenShot/"
    imgDir = "../ScreenShot/"
    imgOri = misc.imread( imgDir + "Demo1.jpg")
    imgGray = misc.imread( imgDir + "Demo1.jpg", flatten = True )
      
    imgT = af.gaussian_filter(imgGray, 0.3)
    imgT = misc.imresize(imgT, [30,30]).astype("float32")
    
    seg = GASegmenter()
    result = seg.segment(imgT)
    #result = misc.imread( imgDir + "label_1_.jpg")[:,:,0]
    result = misc.imresize(result[:,:,0], [imgGray.shape[0],imgGray.shape[1]]).astype("float32")
    misc.imsave(imgDir + "Demo1_regions.jpg", result)
    edge = feature.canny(result, 3, 1.0, 0.3)
    misc.imsave(imgDir + "Demo1_edges.jpg", edge)
    
    for i in xrange(imgOri.shape[0]):
        for j in xrange(imgOri.shape[1]):
            if edge[i,j] == True:
                imgOri[i,j,1] = imgOri[i,j,2] = 0
                imgOri[i,j,0] = 255
                
    misc.imsave(imgDir + "Demo1_result.jpg", imgOri)
   
    
    print "Finish!"