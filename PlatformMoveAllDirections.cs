using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/* This is a script for my final project in my Game Programming class. It is programmed
 * in C# for use in the Unity engine. It is a script that can be applied to platforms in
 * order to have them move about in space. The movement is done via a Lerp with a cosine
 * function This has the effect of having the platform "hang" for a short time on its
 * end position, making it easier for the player to land on, while traveling faster between
 * its start and end position. It also has a collision trigger on the platform, which will
 * add the platform's movement to the player when the player is standing on it. This is so the
 * player will move with the platform. This movement will stack with the movement from the
 * Movement.cs script, so that character movement is affected by both the platform and
 * player input.
 */
public class PlatformMoveAllDirections : MonoBehaviour
{
    public float moveSpeed = 5.0f;
    private Vector3 startPos;
    private Vector3 endPos;
    public float moveX;
    public float moveY;
    public float moveZ;
    private Rigidbody rb;
    CharacterController cc;
    private Vector3 currentPos;
    
    void Start()
    {
        startPos = transform.position;
        endPos = new Vector3(startPos.x + moveX, startPos.y + moveY, startPos.z + moveZ);
        rb = GetComponent<Rigidbody>();
    }

    private void FixedUpdate()
    {
        currentPos = Vector3.Lerp(startPos, endPos, Mathf.Cos(Time.time / moveSpeed * Mathf.PI * 2) * -0.5f + 0.5f);
        rb.MovePosition(currentPos);
    }

    /* As long as the player stands on the platform, the movement of the platform is added to
     * the player. Once the player jumps off of the platform, the movement of the platform
     * stops being added.
     */
    private void OnTriggerEnter(Collider other)
    {
        if (other.name == "Character")
        {
            cc = other.GetComponent<CharacterController>();
        }
    }
    private void OnTriggerStay(Collider other)
    {
        
        if (other.name == "Character")
        {
            cc.Move(rb.velocity * Time.deltaTime);
        }
    }
    
}
