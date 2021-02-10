using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/* This is a script for my final project in my Game Programming class. It is programmed
 * in C# for use in the Unity engine. It is a movement script that is applied to the player 
 * character that lets it move around in a 3d platforming game. The character starts off with 
 * being able to move left, right, forward, and backward. The character can also jump and slam 
 * back down to the ground in midair. After grabbing powerups throughout the level, the character 
 * gains the ability to hover to slow their descent and the ability to jump twice before landing. 
 * There are audio sources in place that play when jumping and hovering respectively.
 */
public class Movement : MonoBehaviour
{
    public float moveSpeed = 5.0f;
    public float mouseSensitivity = 5.0f;
    public float jumpSpeed = 7.0f;
    public float hoverSpeed = -2.0f;
    private bool hover = false;
    float vertRotation = 0;
    public float upDownRange = 60.0f;
    float verticalVelocity = 0;
    int totalJumps = 1;
    int jumpCount = 0;
    AudioSource jumpAudio;
    AudioSource hoverAudio;
    public AudioClip jumpClip;
    public AudioClip hoverClip;
    public GameObject doubleJumpPowerUp;
    public GameObject hoverMsg;
    public GameObject hoverPowerUp;
    public GameObject doubleJumpMsg;
    CharacterController cc;

    void Start()
    {
        Cursor.lockState = CursorLockMode.Locked;
        cc = GetComponent<CharacterController>();
        jumpAudio = gameObject.AddComponent<AudioSource>();
        hoverAudio = gameObject.AddComponent<AudioSource>();
        jumpAudio.volume = 0.5f;
        hoverAudio.volume = 0.5f;
        hoverAudio.spatialBlend = 1;
        jumpAudio.spatialBlend = 1;
        hoverAudio.clip = hoverClip;
        jumpAudio.clip = jumpClip;

    }

    /* A collision detector for the powerups. On collecting a powerup, the
     * corresponding ability is unlocked and the powerup vanishes.
     */
    private void OnTriggerEnter(Collider collider)
    {
        if (collider.name == "Double-Jump Power-up" && totalJumps == 1)
        {
            totalJumps = 2;
            doubleJumpPowerUp.SetActive(false);
            doubleJumpMsg.SetActive(true);
        }
        if (collider.name == "Hover Power-up" && !hover)
        {
            hover = true;
            hoverPowerUp.SetActive(false);
            hoverMsg.SetActive(true);
        }
    }


    void Update()
    {
        /* Pressing 'R' resets the level, so we reset the jumps back to 1.
         */
        if (Input.GetKeyDown(KeyCode.R))
        {
            totalJumps = 1;
        }
        // Pressing 'Esc' quits the game.
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            UnityEditor.EditorApplication.isPlaying = false;
        }
        /* The player camera is controlled by reading the axis inputs from the mouse.
         * The camera can change perspectives during the game, so the player camera only
         * moves when the active camera is the player camera.
         */
        float leftRightRot = Input.GetAxis("Mouse X") * mouseSensitivity;
        transform.Rotate(0, leftRightRot, 0);

        vertRotation -= Input.GetAxis("Mouse Y") * mouseSensitivity;
        vertRotation = Mathf.Clamp(vertRotation, -upDownRange, upDownRange);
        if (Camera.main.enabled) {
            Camera.main.transform.localRotation = Quaternion.Euler(vertRotation, 0, 0);
        }

        //Movement
        float forwardSpeed = 0.0f;
        float sideSpeed = 0.0f;
        if (Camera.main.enabled)
        {
            forwardSpeed = Input.GetAxis("Vertical") * moveSpeed;
            sideSpeed = Input.GetAxis("Horizontal") * moveSpeed;
        }
        /* If the player is currently falling, then they can hover if they have
         * already gotten the powerup.
         */
        if (Input.GetMouseButton(0) && verticalVelocity < 0 && !cc.isGrounded && hover)
        {
            verticalVelocity = hoverSpeed;
            if (jumpAudio.isPlaying)
            {
                jumpAudio.Stop();
            }
            if (!hoverAudio.isPlaying)
            {
               hoverAudio.Play();
            }
            
        } /* If the player is in midair, they can "slam" themselves down to land
           * on platforms faster.
           */
        else if (Input.GetMouseButton(1) && !cc.isGrounded)
        {
            verticalVelocity = -100.0f;
        } /* This sets the vertical velocity in scenarios where the player isn't 
           * hovering or slamming based on the gravity and current vertical velocity.
           * */
        else
        {
            verticalVelocity += Physics.gravity.y * Time.deltaTime;
        }

        // Resets jumps and vertical velocity upon landing
        if(cc.isGrounded)
        {
            verticalVelocity = 0;
            jumpCount = totalJumps;
            if (hoverAudio.isPlaying)
            {
                hoverAudio.Stop();
            }
        }
        /* Jumping is done with the spacebar, and plays the jump audio.
         * Hover and jump audio may be stopped and/or started to make sure there
         * is no overlap with the sounds.
         */
        if(Input.GetButtonDown("Jump") && jumpCount > 0)
        {
            verticalVelocity = jumpSpeed;
            jumpCount--;
            if (hoverAudio.isPlaying)
            {
                hoverAudio.Stop();
            }
            if (jumpAudio.isPlaying)
            {
                jumpAudio.Stop();
                jumpAudio.Play();
            } else
            {
                jumpAudio.Play();
            }
        }
        /* The final movement calculations are made with all 3 components of the speed.
         * That movement is then applied to the player every frame.
         */
        Vector3 speed = new Vector3(sideSpeed, verticalVelocity, forwardSpeed);
        speed = transform.rotation * speed;
        
        cc.Move(speed * Time.deltaTime);
    }
}
