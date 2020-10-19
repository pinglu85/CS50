/* List the titles of all movies in which both Johnny Depp
 and Helena Bonham Carter starred */
SELECT
  title
FROM
  movies
  JOIN stars ON stars.movie_id = movies.id
  JOIN people ON stars.person_id = people.id
WHERE
  name = "Johnny Depp"
  OR name = "Helena Bonham Carter"
GROUP BY
  title
HAVING
  COUNT(title) = 2;