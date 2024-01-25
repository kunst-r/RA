using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

[RequireComponent(typeof(Rigidbody2D))]
public class TeddyController : MonoBehaviour
{
    Rigidbody2D rb;
    private float maxYPosition = 0f;
    private float heightTolerance = 5f;

    public bool movementLeft = false;
    public bool movementRight = false;
    public float movementSpeed = 2.0f;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody2D>();
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKey(KeyCode.A))
        {
            movementLeft = true;
        } else
        {
            movementLeft = false;
        }
        if (Input.GetKey(KeyCode.D))
        {
            movementRight = true;
        } else
        {
            movementRight = false;
        }
        if (Input.GetKey(KeyCode.Escape))
        {
            Application.Quit();
        }
    }

    private void FixedUpdate()
    {
        // fixed speed of left-right movement
        Vector2 position = rb.position;
        if (movementLeft && !movementRight)
        {
            position.x -= movementSpeed * Time.deltaTime;
        } else if (!movementLeft && movementRight) {
            position.x += movementSpeed * Time.deltaTime;
        }
        rb.position = position;

        // if we fall too much down, game over
        if (position.y > maxYPosition)
        {
            maxYPosition = position.y;
        } else if (position.y < maxYPosition - heightTolerance)
        {
            Debug.Log("mrtav");
            GameOver();
        }
    }

    public void GameOver()
    {
        SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex);
    }

    public float GetMaxY()
    {
        return maxYPosition;
    }
}
