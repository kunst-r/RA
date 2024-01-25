using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BeeController : MonoBehaviour
{

    private float movementSpeed = 1f;
    private string movementDirection = "left";
    private Vector3 currentPosition;
    private float leftBorder = -3f;
    private float rightBorder = 3f;

    private Rigidbody2D rb;

    private TeddyController teddyController;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody2D>();
        teddyController = GameObject.Find("teddy").GetComponent<TeddyController>();
    }

    // Update is called once per frame
    void Update()
    {
        currentPosition = transform.position;

        if (movementDirection == "left")
        {
            currentPosition.x -= movementSpeed * Time.deltaTime;
        } else
        {
            currentPosition.x += movementSpeed * Time.deltaTime;
        }
        transform.position = currentPosition;

        if (currentPosition.x < leftBorder)
        {
            movementDirection = "right";
        } else if (currentPosition.x > rightBorder)
        {
            movementDirection = "left";
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.CompareTag("Player"))
        { 
            teddyController.GameOver();
        }
    }

}
