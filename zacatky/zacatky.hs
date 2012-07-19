import Data.List
import Data.Ord
import Data.Array

example :: ([(Int, Int)], [(Int, Int)])
example = ((graphs 8 6) !! 33341)

e1 =  ( generate 6 4 ) !! 5 !! 0 
e2 =  ( generate 6 4 ) !! 5 !! 1 
e3 =  ( generate 6 4 ) !! 7 !! 0 
e4 =  ( generate 6 4 ) !! 7 !! 1 

edges n = [(x,y) | x <- [0..n-1], y <- [0..n-1], x<y]

choose 0 r = [([],r)]
choose _ [] = []
choose n (a:as) = (map (\(x,y)->(a:x,y)) $ choose (n-1) as) ++ (map (\(x,y)->(x,a:y)) $ choose (n) as)

--uplne vsechny grafy
graphs n t = graphs' ((t+1)`div`2) (t`div`2) n
        where graphs' r b n = concat $ map (\(reds,rest) ->  map (\x-> (reds, fst x)) $ choose b rest) $ choose r $ edges n

--ohodnoceni vrcholu jen barevne stupne (konstanty nastavene pro stupne < 10)
evaluate n (a,b) = accumArray (+) 0 (0,n-1) $ sign' 10  b ++ sign' 1000 a
        where sign' t x = map (\x-> (head x, t * (length x))) $ group $ sort $ concat $ map (\(a,b)-> [a,b]) x  

--ohodnoceni vrcholu se stupnemi sousedu
addEdge eval (a,b) = [ (a,(eval!b)`div`10), (b,(eval!a)`div`10) ]
evaluate2 n (a,b) = accumArray (+) 0 (0,n-1) ((assocs (evaluate n (a,b))) ++ (concat $ map (addEdge (evaluate n (a,b))) (a++b)) )

--vyrobi permutaci takovou ze normalizuje graf
normPerm n eval x = array (0,n-1) (identita n ++ perm n x)
        where perm n x = zip (perm' x) (sort $ perm' x)
              perm' x = map fst $ sortBy (comparing snd) $ assocs $ eval n x
              identita n = zip [0..n-1] [0..n-1]

--permutuje graf
sortEdge (a,b) = (min a b, max a b)
permutateGraph  perm (a,b) = (permEdges a, permEdges b)
        where permEdges = sort . (map permEdge)
              permEdge (a,b) = sortEdge (perm!a, perm!b)

--normalizace grafu
norm n eval g = permutateGraph (normPerm n eval g) g
normGraphs n eval graphs = map head $ group $ sort $ map (norm n eval) graphs

--grafy rozdeli tak ze izomorfni jsou ve stejne skupine
sign n eval x = sort $ elems $ eval n x --do ktere skatulky patri
groupGraphs n eval graphs = groupBy (\a b -> sign n eval a == sign n eval b) $ sortBy (comparing (sign n eval)) graphs

--generuje grafy az na eval na n vrcholech po k-tem tahu
generate n k = groupGraphs n evaluate $ normGraphs n evaluate $ graphs n k
generate2 n k = groupGraphs n evaluate2 $ normGraphs n evaluate2 $ graphs n k


--testuje za jsou dva grafy ekvivalenti
ekvialent n a b = or $ map (\c -> c == b) $ map ((flip permutateGraph) a) (all n)
        where all n = map (\p-> array (0,n-1) $ zip p (sort p)) $  permutations [0..n-1]

--nubBy rozdeli na tridy exvivalence
hui n k = concat $ map (nubBy (ekvialent n)) $ generate2 n k

--main = print $ hui 7 6
