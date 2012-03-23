import Data.List
import Data.Ord

depth = 7

edges n = [(x,y) | x <- [0..n-1], y <- [0..n-1], x<y]

choose 0 r = [([],r)]
choose _ [] = []
choose n (a:as) = (map (\(x,y)->(a:x,y)) $ choose (n-1) as) ++ (map (\(x,y)->(x,a:y)) $ choose (n) as)

blue = depth `div` 2
red = depth - blue

graphs r b n = concat $ map (\(reds,rest) ->  map (\x-> (reds, fst x)) $ choose b rest) $ choose r $ edges n

f x = map length $ group $ sort $ concat $ map (\(a,b)-> [a,b]) x --vyroby stupne jednobarevneho grafu
sign (a,b) = (f a, f b)

hui x = map length $ groupBy (\a b -> sign a == sign b) $ sortBy (comparing sign) x


--g x = head $ group $ sort $ concat $ map (\(a,b)-> [a,b]) x 
g x = map (\x-> (head x, length x)) $ group $ sort $ concat $ map (\(a,b)-> [a,b]) x 
p (a,b) = groupBy (\x y-> fst a == fst b) $ sort $ g b ++ ( map (\(x,y)->(x,y*100)) $ g a)
