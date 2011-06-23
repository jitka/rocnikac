import Data.List
import Data.Ord
import Data.Function
import Data.Array
import Control.DeepSeq

vrcholu = 18

-- pomocny binec..
nad :: (Integral a) => a -> a -> a
nad a 0 = 1
nad a b | a >= b = (a * ((a-1) `nad` (b-1))) `div` b

jePrvocislo x = and $ ( (x>1) : ) $ map (/=0) $ map ( x `mod` ) $ takeWhile ( \a -> a*a <= x ) [2..]

prvocisla = filter jePrvocislo [2..]

-- rozklad :: Integereger -> [Integereger]
rozklad = rozklad' prvocisla
        where 
--        rozklad' :: [Integereger] -> Integereger -> [Integereger]
        rozklad' _ 1 = []
        rozklad' (a:zbytek) n | n `mod` a == 0 = a : rozklad' (a:zbytek) (n `div` a)
                              | otherwise = rozklad' zbytek n

cifernyRozkladBy z = reverse . unfoldr (\x -> if x>0 then Just(x`mod`z,x`div`z) else Nothing)
cifernyRozklad = cifernyRozkladBy 10

scvrkavani :: (Eq a) => [a] -> [(a,Integer)] --setrizenych
scvrkavani = map (\a -> ( head a, genericLength a )) . group 

scvrkavani2 :: (Ord a) => [(a,Integer)] -> [(a,Integer)] --setrizenych
scvrkavani2 = map (foldl1 (\(z,p1) (_,p2) -> (z,p1+p2))) . groupBy ((==) `on` fst) . sort

l = length . cifernyRozklad 

nsd a b | b == 0 = a
        | otherwise = nsd b (a `mod` b)

nsn a b = (a*b) `div` (nsd a b)

--generuje vsechny setrizene zpusoby jak nastitat k
rozdel n = rozdel' 1 n
        where
        rozdel' _ 0 = [[]]
        rozdel' k n | k > n = []
                    | otherwise = map (k:) (rozdel' k (n-k)) ++ rozdel' (k+1) n


-- !!!!TU TO ZACINA!!!!
-- budu se snazit zjistit neco o tom kolik ma hra stavu
-- pokracujici hra je takova, kdyz se nezastavi pri vyteztvi nekoho a pokracuje se dal
-- zacina zeleny hrac a pak hraje cerveny

-- pocet stavu pokracujici hry po k-tem tahu
f1 n tah = let pul = ((tah+1) `div` 2)
               hran = n `nad` 2 
           in (hran `nad` pul) * ( (hran-pul) `nad` (tah-pul) )
--je:
f2 = map (f1 vrcholu) [0..(vrcholu `nad` 2)]
--a to nejvice po tahu:
f3 = maximumBy (comparing (f1 vrcholu)) [0..(vrcholu `nad` 2)]

--celkove je stavu pokracujici hry, logaritmus:
f4 = (sum f2, l $ sum f2)

--
--zpocitam stavy pokracujici hry az na izomorfizmus z Burnsideova lemmatu
--

--jak dopadne obarveni hran jejiz oba vrchyly lezi na cyklu permutace


-- k je velikost cyklu, krok udava typ hrany pres kolik vrcholu v cyklu je
k1 :: Integer -> [((Integer,Integer),Integer)]
k1 k | k `mod` 2 == 1 = k2 $ replicate (fromIntegral k `div` 2) k
     | otherwise = k2 $ (k`div`2) : replicate (fromIntegral (k-1) `div` 2) k

--jak muze dopadnout obarvovani skupinek hran
--(batoh s malymi cistly)
--k2' dostane vsechny moznosti ((co_to_stoji_zelenych,cervenych),kolika_zpusoby_to_jde_udelat)
--a jeddnu skupinku hrana tu zakomponuje

k2 :: [Integer] -> [((Integer,Integer),Integer)]
k2 skupinky = foldl k2' [((0,0),1)] skupinky

k2' :: [((Integer,Integer),Integer)] -> Integer -> [((Integer,Integer),Integer)]
k2' puvodni s = scvrkavani2 $ (map (\((z,c),p) -> ((z,c+s),p) ) puvodni) ++ (map (\((z,c),p) -> ((z+s,c),p) ) puvodni) ++ puvodni

nasobek :: [((Integer,Integer),Integer)] -> [((Integer,Integer),Integer)] -> [((Integer,Integer),Integer)]
nasobek a b = scvrkavani2 $ [ ((z1+z2,c1+c2),p1*p2) | ((z1,c1),p1) <- a, ((z2,c2),p2) <- b ]

--pomoci kolika barev kolika zpusoby jde vytvorit fixpinty vuci dane permutaci
--k3 :: [Integer] -> [((Integer,Integer),Integer)] -> [((Integer,Integer),Integer)]
k3 [] = [((0,0),1)]
k3 (vel:permutace) = 
        let --vsechny zbytky po obarveni prvniho cyklu
            obarveni1 = k1 vel
            --skupiny hran mezi prvnim a dalsimy cykly, ktere musi mit stejnou barvu
            hrany = reverse $ sort $ concat $ map (\x -> replicate (fromIntegral $ nsd vel x) (nsn vel x)) permutace
            --vsechny obarveni techto hran
            obarveni2 = k2 hrany
            --dohromady vsechny obarveni
            obarveni = nasobek obarveni1 obarveni2 
        in nasobek obarveni $ k3 permutace

--kolik fixpointu ma hra v nejakem tahu pro nejakou permutaci 
k4 tah permutace = snd $ head $ (filter (\((z,c),_) -> ( z==((tah+1)`div`2) && c==(tah`div`2) )) $ k3 permutace) ++ [((undefined,undefined),0)]

--pocet stavu hry po nejakem tahu az na izomorfizmus
f5 n tah = (sum $ map (\per -> (k4 tah per)*(product per)) $ rozdel n) `div` (product [1..n])

sum' = foldl' (+) 0

f6 n = let tabulka = map (\p -> (p,k3 p)) $ rozdel n
           vytahni' permutace = snd $ head $ filter (\x->fst x == permutace) tabulka
           barvy tah = ((tah+1)`div`2, tah`div`2)
           vytahni tah permutace = snd $ head $ (filter (\(b,_) -> ( b == barvy tah )) $ vytahni' permutace) ++ [((undefined,undefined),0)]
           f5' n tah = (sum' $ map (\per -> (vytahni tah per)*(product per)) $ rozdel n) `div` (product [1..n])
        in tabulka `deepseq` map (\tah -> (tah,l $ f1 n tah, l $ f5' n tah )) [1..(n`nad`2)]

main = print $ f6 12
