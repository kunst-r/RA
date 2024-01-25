using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class CameraMovement : MonoBehaviour
{
    // camera rotation
    float rotationX = 0f;
    float rotationY = 0f;
    public float sensitivity = 3f;

    // camera movement
    private Vector3 CameraPosition;
    public float CameraSpeed = 0.01f;

    // Update is called once per frame
    void Update()
    {
        // camera rotation
        rotationY += Input.GetAxis("Mouse X") * sensitivity;
        rotationX += Input.GetAxis("Mouse Y") * sensitivity * (-1);
        transform.localEulerAngles = new Vector3(rotationX, rotationY, 0);

        // camera movement speed
        if (Input.GetKey(KeyCode.LeftShift))
        {
            CameraSpeed = 0.05f;
        }
        else
        {
            CameraSpeed = 0.01f;

        }

        // camera movement
        if (Input.GetKey(KeyCode.W))
        {
            this.transform.position += this.transform.forward * CameraSpeed;
        }
        if (Input.GetKey(KeyCode.S))
        {
            this.transform.position -= this.transform.forward * CameraSpeed;
        }
        if (Input.GetKey(KeyCode.A))
        {
            this.transform.position -= this.transform.right * CameraSpeed;
        }
        if (Input.GetKey(KeyCode.D))
        {
            this.transform.position += this.transform.right * CameraSpeed;
        }
    }
}
