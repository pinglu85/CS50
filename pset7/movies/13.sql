/* List the names of all people who starred in a movie in which Kevin Bacon also starred */
SELECT
  name
FROM
  people
  JOIN stars ON stars.person_id = people.id
  JOIN movies ON stars.movie_id = movies.id
WHERE
  movies.id IN (
    SELECT
      movies.id
    FROM
      movies
      JOIN stars ON stars.movie_id = movies.id
      JOIN people ON stars.person_id = people.id
    WHERE
      name = "Kevin Bacon"
      AND birth = 1958
  )
  AND name != "Kevin Bacon"
GROUP BY
  people.id;